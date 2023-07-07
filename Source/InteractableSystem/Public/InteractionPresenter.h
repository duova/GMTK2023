// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InteractionPresenter.generated.h"

class UInteractableComponent;
class UInteractorComponent;

/**
 * Script for determining how different interaction elements are to be presented.
 */
UCLASS(Blueprintable)
class INTERACTABLESYSTEM_API UInteractionPresenter : public UObject
{
	GENERATED_BODY()

public:

	UInteractionPresenter();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitializeUI(UInteractorComponent* Interactor, UInteractableComponent* Interactable);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayText(const FText& Text, UInteractorComponent* Interactor, UInteractableComponent* Interactable);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayChoice(const FText& Text, const TArray<FText>& Choices, UInteractorComponent* Interactor, UInteractableComponent* Interactable);

	UFUNCTION(BlueprintImplementableEvent)
	void OnClearUI(UInteractorComponent* Interactor, UInteractableComponent* Interactable);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCloseUI(UInteractorComponent* Interactor, UInteractableComponent* Interactable);
};
