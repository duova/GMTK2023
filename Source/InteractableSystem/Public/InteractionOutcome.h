// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InteractionOutcome.generated.h"

class UInteractorComponent;

/**
 * A definition for an outcome of an interaction.
 */
UCLASS(Blueprintable)
class INTERACTABLESYSTEM_API UInteractionOutcome : public UObject
{
	GENERATED_BODY()

public:

	UInteractionOutcome();

	//Event called when the outcome is achieved.
	UFUNCTION(BlueprintImplementableEvent)
	void OnExecute(UInteractorComponent* Interactor);
};
