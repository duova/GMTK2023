// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InteractionGroup.generated.h"

class UInteractableComponent;
class UInteractionOutcome;
class UInteractorComponent;

UENUM(BlueprintType)
enum class EInteractionChoice : uint8
{
	Choice0,
	Choice1,
	Choice2,
	Choice3,
	Choice4,
	Choice5
};

UENUM(BlueprintType)
enum class ECurrentInteractionNodeType : uint8
{
	Text,
	Choice
};

/**
 * A set of interaction elements that can be initiated by the InteractableComponent.
 * In blueprints this is defined in a tree structure for easier understanding.
 */
UCLASS(Blueprintable)
class INTERACTABLESYSTEM_API UInteractionGroup : public UObject
{
	GENERATED_BODY()

public:

	UInteractionGroup();

	void Initialize(UInteractorComponent* InInteractorComponent, UInteractableComponent* InInteractableComponent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnInteractionTree();

	UFUNCTION(BlueprintCallable)
	void DisplayText(const FText Text);

	UFUNCTION(BlueprintCallable, Meta = (ExpandEnumAsExecs = "InteractionChoice"))
	void PresentChoice(const FText Text, const FText Choice0, const FText Choice1, const FText Choice2, const FText Choice3, const FText Choice4, const FText Choice5, EInteractionChoice& InteractionChoice);

	//Outcome is optional.
	UFUNCTION(BlueprintCallable)
	void EndBranch(TSubclassOf<UInteractionOutcome> OutcomeClass);

	UFUNCTION(BlueprintCallable)
	UInteractorComponent* GetInteractorComponent();
	
	void Continue();
	
	void Leave();

	void Select(const uint8 Choice);

private:
	uint8 NumberOfInteractionNodesTraversed = 0;
	
	uint8 Iterator = 0;

	//Lowest is first, highest is latest. Zero filled if not a choice.
	TArray<uint8> ChoicesSelected;

	ECurrentInteractionNodeType CurrentNodeType;

	UPROPERTY()
	UInteractorComponent* InteractorComponent = nullptr;
	
	UPROPERTY()
	UInteractableComponent* InteractableComponent = nullptr;
};
