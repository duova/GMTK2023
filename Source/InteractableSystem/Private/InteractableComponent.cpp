// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "InteractionGroup.h"
#include "InteractionOutcome.h"
#include "InteractorComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::Interact(UInteractorComponent* Interactor)
{
	if (!CanInteract()) return;
	OnInteract(Interactor);
	Interactor->LastInteraction = this;
}

void UInteractableComponent::EnableInteraction()
{
	bInteractionAvailable = true;
}

void UInteractableComponent::DisableInteraction()
{
	bInteractionAvailable = false;
}

bool UInteractableComponent::CanInteract()
{
	return bInteractionAvailable;
}

void UInteractableComponent::StartInteractionGroup(const TSubclassOf<UInteractionGroup> InteractionGroupClass, UInteractorComponent* Interactor)
{
	if (InteractionGroup != nullptr) return;
	InteractionGroup = NewObject<UInteractionGroup>(this, InteractionGroupClass);
	InteractionGroup->Initialize(Interactor, this);
}

void UInteractableComponent::ProduceOutcome(const TSubclassOf<UInteractionOutcome> OutcomeClass,
	UInteractorComponent* Interactor)
{
	if (OutcomeClass != nullptr) OutcomeClass.GetDefaultObject()->OnExecute(Interactor);
}

void UInteractableComponent::StopInteractionGroup()
{
	InteractionGroup = nullptr;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	if (MainInteractor != nullptr)
	{
		MainInteractor->OnStateDirtyDelegate.AddUObject(this, &UInteractableComponent::OnStateUpdate);
	}
}

