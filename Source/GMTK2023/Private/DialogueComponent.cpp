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
	RemainingNeutralQuestions.Empty();
	RemainingOptimalQuestions.Empty();
	RemainingBadQuestions.Empty();

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
		if (CurrentQuestionTable[i]->Value == 0)
		{
			RemainingNeutralQuestions.Add(i);
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
	const uint8 LowestNumberOfRequiredQuestions = FMath::Floor(CurrentCharacterData->NumberOfQuestionsInASet / 3) + 1;
	if (RemainingNeutralQuestions.Num() < LowestNumberOfRequiredQuestions ||
		RemainingOptimalQuestions.Num() < LowestNumberOfRequiredQuestions ||
		RemainingBadQuestions.Num() < LowestNumberOfRequiredQuestions ||
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
		if (PickedQuestion->Value < 0)
		{
			SuspicionMeter -= PickedQuestion->Value;
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
	const uint8 NumberOfQuestionsFromEachType = FMath::Floor(CurrentCharacterData->NumberOfQuestionsInASet / 3);
	const uint8 NumberOfQuestionsFromAnyType = CurrentCharacterData->NumberOfQuestionsInASet % 3;
	TArray<FQuestionTableRow*> OrderedResult;
	for (uint8 i = 0; i < NumberOfQuestionsFromEachType; i++)
	{
		AddRandomQuestionToArrayAndRemove(RemainingNeutralQuestions, OrderedResult);
		AddRandomQuestionToArrayAndRemove(RemainingOptimalQuestions, OrderedResult);
		AddRandomQuestionToArrayAndRemove(RemainingBadQuestions, OrderedResult);
	}
	for (uint8 i = 0; i < NumberOfQuestionsFromAnyType; i++)
	{
		const uint8 TypeRoll = FMath::RandRange(0, 2);
		if (TypeRoll == 0)
		{
			AddRandomQuestionToArrayAndRemove(RemainingNeutralQuestions, OrderedResult);
		}
		else if (TypeRoll == 1)
		{
			AddRandomQuestionToArrayAndRemove(RemainingOptimalQuestions, OrderedResult);
		}
		else
		{
			AddRandomQuestionToArrayAndRemove(RemainingBadQuestions, OrderedResult);
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
