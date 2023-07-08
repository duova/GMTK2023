// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueComponent.generated.h"

struct FQuestionTableRow;
class UDataTable;

UENUM()
enum class EInterviewState
{
	None,
	Start,
	Question,
	Answer,
	Guess,
	Bust,
	End
};

UCLASS(EditInlineNew)
class GMTK2023_API UDialogueCharacterData : public UObject
{
	GENERATED_BODY()

public:
	
	UDialogueCharacterData();

	UPROPERTY(EditAnywhere)
	FText Title;
	
	UPROPERTY(EditAnywhere)
	UDataTable* QuestionTable;

	UPROPERTY(EditAnywhere)
	FText StartingText;

	UPROPERTY(EditAnywhere)
	uint8 NumberOfQuestionsInASet;

	UPROPERTY(EditAnywhere)
	uint8 NumberOfQuestionSets;

	UPROPERTY(EditAnywhere)
	FText BustText;

	UPROPERTY(EditAnywhere)
	FText EndingCorrectText;

	UPROPERTY(EditAnywhere)
	FText EndingIncorrectText;

	//Meter goes up by 1 - value each question.
	UPROPERTY(EditAnywhere)
	float BustValue;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class GMTK2023_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TArray<UDialogueCharacterData*> CharacterData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 NumberOfCharacterGuessOptions;

	UPROPERTY(BlueprintReadOnly)
	uint8 NumberOfCorrectGuesses;

	UPROPERTY(BlueprintReadOnly)
	uint8 NumberOfInterviewsCompleted;

	UPROPERTY(BlueprintReadOnly)
	uint8 NumberOfInterviewsStarted;

	UFUNCTION(BlueprintCallable)
	void StartInterview();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInterviewStart(const FText& StartingText);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUIClear();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayQuestions(const TArray<FText>& QuestionOptions);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayAnswer(const FText& Answer, const float NewMeterValueInDecimal);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayOptions(const TArray<FText>& CharacterTitleOptions);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayEndingText(const FText& EndingText, bool bCorrect);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayBust(const FText& BustText);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInterviewEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnOutOfInterviews();

	UFUNCTION(BlueprintGetter)
	void GetStats(int32& OutNumberOfCorrectGuesses, int32& OutNumberOfInterviewsCompleted, int32& OutNumberOfInterviewsStarted) const;
	
	UFUNCTION(BlueprintCallable)
	void Continue();

	//Zero-indexed.
	UFUNCTION(BlueprintCallable)
	void Select(int32 Option);

private:

	TArray<FQuestionTableRow*> SelectAndRemoveQuestions();

	void AddRandomQuestionToArrayAndRemove(TArray<uint8>& QuestionIndexArray, TArray<FQuestionTableRow*>& ArrayToAddTo);

	UPROPERTY()
	TArray<UDialogueCharacterData*> RemainingCharacters;

	UPROPERTY()
	UDialogueCharacterData* CurrentCharacterData;
	
	TArray<FQuestionTableRow*> CurrentQuestionTable;
	
	TArray<FQuestionTableRow*> SelectedQuestions;

	UPROPERTY()
	TArray<uint8> RemainingOptimalQuestions;
	
	UPROPERTY()
	TArray<uint8> RemainingNeutralQuestions;

	UPROPERTY()
	TArray<uint8> RemainingBadQuestions;

	UPROPERTY()
	float SuspicionMeter;

	UPROPERTY()
	uint8 QuestionsDone;

	EInterviewState InterviewState;

	UPROPERTY()
	TArray<UDialogueCharacterData*> ShownCharacterGuessOptions;
};
