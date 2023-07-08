// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent.h"

#include "QuestionTableRow.h"
#include "Engine/DataTable.h"

UDialogueCharacterData::UDialogueCharacterData()
{
}

UDialogueComponent::UDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	RemainingCharacters = CharacterData;
	NumberOfCorrectGuesses = 0;
	NumberOfInterviewsCompleted = 0;
	NumberOfInterviewsStarted = 0;
}

void UDialogueComponent::StartInterview()
{
	RemainingNormalQuestions.Empty();
	RemainingOptimalQuestions.Empty();

	if (RemainingCharacters.Num() <= 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
			                                 TEXT("Tried to start interview with no interviews available!"));
		return;
	}

	CurrentCharacterData = RemainingCharacters[FMath::RandRange(0, RemainingCharacters.Num() - 1)];
	RemainingCharacters.Remove(CurrentCharacterData);

	CurrentQuestionTable.Empty();
	CurrentCharacterData->QuestionTable->GetAllRows<FQuestionTableRow>(FString(), CurrentQuestionTable);

	for (uint8 i = 0; i < CurrentQuestionTable.Num(); i++)
	{
		if (CurrentQuestionTable[i]->Value < 1)
		{
			RemainingNormalQuestions.Add(i);
		}
		else
		{
			RemainingOptimalQuestions.Add(i);
		}
	}

	SuspicionMeter = 0;
	QuestionsDone = 0;
	NumberOfInterviewsStarted++;
	SelectedQuestions.Empty();
	InterviewState = EInterviewState::Start;

	OnInterviewStart(CurrentCharacterData->StartingText);
}

void UDialogueComponent::GetStats(int32& OutNumberOfCorrectGuesses, int32& OutNumberOfInterviewsCompleted,
	int32& OutNumberOfInterviewsStarted) const
{
	OutNumberOfCorrectGuesses = NumberOfCorrectGuesses;
	OutNumberOfInterviewsCompleted = NumberOfInterviewsCompleted;
	OutNumberOfInterviewsStarted = NumberOfInterviewsStarted;
}

void UDialogueComponent::Continue()
{
	if (InterviewState == EInterviewState::Bust)
	{
		OnInterviewEnd();
		if (RemainingCharacters.Num() <= 0)
		{
			OnOutOfInterviews();
		}
		InterviewState = EInterviewState::None;
	}
	else if (InterviewState == EInterviewState::End)
	{
		OnInterviewEnd();
		if (RemainingCharacters.Num() <= 0)
		{
			OnOutOfInterviews();
		}
		InterviewState = EInterviewState::None;
	}
	if (!(InterviewState == EInterviewState::Start || InterviewState == EInterviewState::Answer)) return;
	if (RemainingNormalQuestions.Num() < CurrentCharacterData->NumberOfNormalQuestionsGiven ||
		RemainingOptimalQuestions.Num() < CurrentCharacterData->NumberOfOptimalQuestionsGiven || QuestionsDone >=
		CurrentCharacterData->NumberOfQuestionSets)
	{
		//No more questions.
		OnUIClear();
		InterviewState = EInterviewState::Guess;
		NumberOfInterviewsCompleted++;
		TArray<UDialogueCharacterData*> PossibleCharacters = CharacterData;
		ShownCharacterGuessOptions.Empty();
		for (uint8 i = 0; i < NumberOfCharacterGuessOptions - 1; i++)
		{
			UDialogueCharacterData* ToAdd = PossibleCharacters[FMath::RandRange(0, PossibleCharacters.Num() - 1)];
			ShownCharacterGuessOptions.Add(ToAdd);
			PossibleCharacters.Remove(ToAdd);
		}
		ShownCharacterGuessOptions.Insert(CurrentCharacterData, FMath::RandRange(0, ShownCharacterGuessOptions.Num()));
		TArray<FText> ShownCharacterGuessTitles;
		for (UDialogueCharacterData* Character : ShownCharacterGuessOptions)
		{
			ShownCharacterGuessTitles.Add(Character->Title);
		}
		OnDisplayOptions(ShownCharacterGuessTitles);
	}
	else
	{
		//Give questions.
		OnUIClear();
		SelectedQuestions = SelectAndRemoveQuestions();
		TArray<FText> TextQuestions;
		for (const FQuestionTableRow* Row : SelectedQuestions)
		{
			TextQuestions.Add(Row->Question);
		}
		InterviewState = EInterviewState::Question;
		OnDisplayQuestions(TextQuestions);
	}
}

void UDialogueComponent::Select(const int32 Option)
{
	if (InterviewState == EInterviewState::Question)
	{
		if (SelectedQuestions.Num() <= Option) return;
		OnUIClear();
		const FQuestionTableRow* PickedQuestion = SelectedQuestions[Option];
		if (PickedQuestion->Value < 0.5)
		{
			SuspicionMeter += 0.5 - PickedQuestion->Value;
		}
		if (SuspicionMeter > CurrentCharacterData->BustValue)
		{
			InterviewState = EInterviewState::Bust;
			OnDisplayBust(CurrentCharacterData->BustText);
		}
		else
		{
			InterviewState = EInterviewState::Answer;
			QuestionsDone++;
			OnDisplayAnswer(PickedQuestion->Answer, SuspicionMeter / CurrentCharacterData->BustValue);
		}
	}
	else if (InterviewState == EInterviewState::Guess)
	{
		if (ShownCharacterGuessOptions.Num() <= Option) return;
		OnUIClear();
		const UDialogueCharacterData* PickedCharacter = ShownCharacterGuessOptions[Option];
		if (PickedCharacter == CurrentCharacterData)
		{
			InterviewState = EInterviewState::End;
			NumberOfCorrectGuesses++;
			OnDisplayEndingText(CurrentCharacterData->EndingCorrectText, true);
		}
		else
		{
			InterviewState = EInterviewState::End;
			OnDisplayEndingText(CurrentCharacterData->EndingIncorrectText, false);
		}
	}
}

TArray<FQuestionTableRow*> UDialogueComponent::SelectAndRemoveQuestions()
{
	TArray<FQuestionTableRow*> Result;
	uint8 IndexSelected;
	for (uint8 i = 0; i < CurrentCharacterData->NumberOfNormalQuestionsGiven; i++)
	{
		IndexSelected = RemainingNormalQuestions[FMath::RandRange(0, RemainingNormalQuestions.Num() - 1)];
		Result.Add(CurrentQuestionTable[IndexSelected]);
		CurrentQuestionTable.RemoveAt(IndexSelected);
	}
	for (uint8 i = 0; i < CurrentCharacterData->NumberOfOptimalQuestionsGiven; i++)
	{
		IndexSelected = RemainingOptimalQuestions[FMath::RandRange(0, RemainingOptimalQuestions.Num() - 1)];
		Result.Add(CurrentQuestionTable[IndexSelected]);
		CurrentQuestionTable.RemoveAt(IndexSelected);
	}
	return Result;
}
