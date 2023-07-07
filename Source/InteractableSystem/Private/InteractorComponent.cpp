// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorComponent.h"

#include "InteractableComponent.h"
#include "InteractionGroup.h"

UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractorComponent::Continue() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Continue();
	}
}

void UInteractorComponent::Leave() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Leave();
	}
}

void UInteractorComponent::Select0() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(0);
	}
}

void UInteractorComponent::Select1() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(1);
	}
}

void UInteractorComponent::Select2() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(2);
	}
}

void UInteractorComponent::Select3() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(3);
	}
}

void UInteractorComponent::Select4() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(4);
	}
}

void UInteractorComponent::Select5() const
{
	if (LastInteraction != nullptr && LastInteraction->InteractionGroup != nullptr)
	{
		LastInteraction->InteractionGroup->Select(5);
	}
}

void UInteractorComponent::StateDirty()
{
	OnStateDirtyDelegate.Broadcast(this);
}

