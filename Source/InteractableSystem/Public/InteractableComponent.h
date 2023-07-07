// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

class UInteractionOutcome;
class UInteractionGroup;

/**
 * Component containing functionality that allows an object to be "interacted" with.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INTERACTABLESYSTEM_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UInteractorComponent;

public:	
	UInteractableComponent();

	UFUNCTION(BlueprintCallable)
	void Interact(UInteractorComponent* Interactor);

	//Called when main interactor's state dirty function is called.
	UFUNCTION(BlueprintImplementableEvent)
	void OnStateUpdate(UInteractorComponent* Interactor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteract(UInteractorComponent* Interactor);

	UFUNCTION(BlueprintCallable)
	void EnableInteraction();

	UFUNCTION(BlueprintCallable)
	void DisableInteraction();

	UFUNCTION(BlueprintGetter)
	bool CanInteract();

	UFUNCTION(BlueprintCallable)
	void StartInteractionGroup(const TSubclassOf<UInteractionGroup> InteractionGroupClass, UInteractorComponent* Interactor);

	UFUNCTION(BlueprintCallable)
	void ProduceOutcome(const TSubclassOf<UInteractionOutcome> OutcomeClass, UInteractorComponent* Interactor);
	
	void StopInteractionGroup();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInteractorComponent* MainInteractor;

	virtual void BeginPlay() override;
	
private:
	bool bInteractionAvailable = true;

	UPROPERTY()
	UInteractionGroup* InteractionGroup;
};
