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

	LastPlayedCharacter = nullptr;
	NumberOfCorrectGuesses = 0;
	NumberOfInterviewsCompleted = 0;
	NumberOfInterviewsStarted = 0;
	SuspicionMeter = 0;
}

void UDialogueComponent::StartInterview()
{
	RemainingGeneralQuestions.Empty();
	RemainingOptimalQuestions.Empty();
	RemainingBadQuestions.Empty();

	if (CharacterData.Num() <= 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
			                                 TEXT("Tried to start interview with no interviews available!"));
		return;
	}

	TArray<UDialogueCharacterData*> AvailableCharacters = CharacterData;
	if (LastPlayedCharacter)
	{
		AvailableCharacters.Remove(LastPlayedCharacter);
	}
	
	CurrentCharacterData = AvailableCharacters[FMath::RandRange(0, AvailableCharacters.Num() - 1)];

	CurrentQuestionTable.Empty();
	CurrentCharacterData->QuestionTable->GetAllRows<FQuestionTableRow>(FString(), CurrentQuestionTable);

	for (uint8 i = 0; i < CurrentQuestionTable.Num(); i++)
	{
		if (CurrentQuestionTable[i]->Value == 0)
		{
			RemainingGeneralQuestions.Add(i);
		}
		else if (CurrentQuestionTable[i]->Value > 0)
		{
			RemainingOptimalQuestions.Add(i);
		}
		else
		{
			RemainingBadQuestions.Add(i);
		}
	}
	
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

void UDialogueComponent::Reset()
{
	OnUIClear();
	OnInterviewEnd();
	LastPlayedCharacter = nullptr;
	NumberOfCorrectGuesses = 0;
	NumberOfInterviewsCompleted = 0;
	NumberOfInterviewsStarted = 0;
	SuspicionMeter = 0;

	RemainingGeneralQuestions.Empty();
	RemainingOptimalQuestions.Empty();
	RemainingBadQuestions.Empty();

	QuestionsDone = 0;
	SelectedQuestions.Empty();
	InterviewState = EInterviewState::None;
}

void UDialogueComponent::Continue()
{
	if (InterviewState == EInterviewState::Bust)
	{
		OnInterviewEnd();
		InterviewState = EInterviewState::None;
	}
	else if (InterviewState == EInterviewState::End)
	{
		OnInterviewEnd();
		InterviewState = EInterviewState::None;
	}
	if (!(InterviewState == EInterviewState::Start || InterviewState == EInterviewState::Answer)) return;
	if (RemainingGeneralQuestions.Num() < CurrentCharacterData->NumberOfGeneralQuestionsPerSet ||
		RemainingOptimalQuestions.Num() + RemainingBadQuestions.Num() < CurrentCharacterData->NumberOfSpecificQuestionsPerSet ||
		QuestionsDone >= CurrentCharacterData->NumberOfQuestionSets)
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
		TArray<FText> ShownCareersOfCharactersForGuessing;
		for (const UDialogueCharacterData* Character : ShownCharacterGuessOptions)
		{
			ShownCareersOfCharactersForGuessing.Add(Character->Career);
		}
		OnDisplayOptions(ShownCareersOfCharactersForGuessing);
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
		SuspicionMeter -= PickedQuestion->Value;
		if (SuspicionMeter > CurrentCharacterData->BustValue)
		{
			InterviewState = EInterviewState::Bust;
			SuspicionMeter = 0;
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
	TArray<FQuestionTableRow*> OrderedResult;
	
	for (uint8 i = 0; i < CurrentCharacterData->NumberOfGeneralQuestionsPerSet; i++)
	{
		AddRandomQuestionToArrayAndRemove(RemainingGeneralQuestions, OrderedResult);
	}
	for (uint8 i = 0; i < CurrentCharacterData->NumberOfSpecificQuestionsPerSet; i++)
	{
		if (RemainingBadQuestions.Num() <= 0)
		{
			AddRandomQuestionToArrayAndRemove(RemainingOptimalQuestions, OrderedResult);
		}
		else if (RemainingOptimalQuestions.Num() <= 0)
		{
			AddRandomQuestionToArrayAndRemove(RemainingBadQuestions, OrderedResult);
		}
		else
		{
			const bool TypeRoll = FMath::RandBool();
			if (TypeRoll)
			{
				AddRandomQuestionToArrayAndRemove(RemainingBadQuestions, OrderedResult);
			}
			else
			{
				AddRandomQuestionToArrayAndRemove(RemainingOptimalQuestions, OrderedResult);
			}
		}
	}
	TArray<FQuestionTableRow*> ShuffledResult;
	for (uint8 i = 0; i < OrderedResult.Num(); i++)
	{
		FQuestionTableRow* Selected = OrderedResult[FMath::RandRange(0, OrderedResult.Num() - 1)];
		ShuffledResult.Add(Selected);
		OrderedResult.Remove(Selected);
	}
	return ShuffledResult;
}

void UDialogueComponent::AddRandomQuestionToArrayAndRemove(TArray<uint8>& QuestionIndexArray,
	TArray<FQuestionTableRow*>& ArrayToAddTo)
{
	const uint8 IndexSelected = QuestionIndexArray[FMath::RandRange(0, QuestionIndexArray.Num() - 1)];
	ArrayToAddTo.Add(CurrentQuestionTable[IndexSelected]);
	CurrentQuestionTable.RemoveAt(IndexSelected);
}
