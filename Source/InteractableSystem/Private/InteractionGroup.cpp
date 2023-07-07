// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionGroup.h"

#include "InteractableComponent.h"
#include "InteractionOutcome.h"
#include "InteractionPresenter.h"
#include "InteractorComponent.h"

UInteractionGroup::UInteractionGroup()
{
}

void UInteractionGroup::Initialize(UInteractorComponent* InInteractorComponent, UInteractableComponent* InInteractableComponent)
{
	if (InInteractorComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractorComponent is null for InteractionGroup."));
	}
	if (InInteractableComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractableComponent is null for InteractionGroup."));
	}
	InteractorComponent = InInteractorComponent;
	InteractableComponent = InInteractableComponent;
	if (InteractorComponent->InteractionPresenterClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionPresenterClass in InteractionComponent is null."));
		return;
	}
	InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnInitializeUI(InteractorComponent, InteractableComponent);
	OnInteractionTree();
}

void UInteractionGroup::DisplayText(const FText Text)
{
	if (Iterator == NumberOfInteractionNodesTraversed)
	{
		InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnDisplayText(Text, InteractorComponent, InteractableComponent);
		CurrentNodeType = ECurrentInteractionNodeType::Text;
		//Zero fill choice selected.
		ChoicesSelected.Emplace(0);
	}
	Iterator++;
}

void UInteractionGroup::PresentChoice(const FText Text, const FText Choice0, const FText Choice1, const FText Choice2, const FText Choice3,
	const FText Choice4, const FText Choice5, EInteractionChoice& InteractionChoice)
{
	if (Iterator == NumberOfInteractionNodesTraversed)
	{
		InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnDisplayChoice(
			Text, TArray<FText>{Choice0, Choice1, Choice2, Choice3, Choice4, Choice5}, InteractorComponent,
			InteractableComponent);
		InteractionChoice = EInteractionChoice::Choice0;
		CurrentNodeType = ECurrentInteractionNodeType::Choice;
	}
	else if (Iterator < NumberOfInteractionNodesTraversed)
	{
		InteractionChoice = static_cast<EInteractionChoice>(ChoicesSelected[Iterator]);
	}
	Iterator++;
}

void UInteractionGroup::EndBranch(TSubclassOf<UInteractionOutcome> OutcomeClass)
{
	if (Iterator == NumberOfInteractionNodesTraversed)
	{
		if (OutcomeClass != nullptr) OutcomeClass.GetDefaultObject()->OnExecute(InteractorComponent);
		Leave();
	}
	Iterator++;
}

UInteractorComponent* UInteractionGroup::GetInteractorComponent()
{
	return InteractorComponent;
}

void UInteractionGroup::Continue()
{
	if (CurrentNodeType == ECurrentInteractionNodeType::Text)
	{
		InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnClearUI(InteractorComponent, InteractableComponent);
		NumberOfInteractionNodesTraversed++;
		Iterator = 0;
		OnInteractionTree();
	}
}

void UInteractionGroup::Leave()
{
	InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnCloseUI(InteractorComponent, InteractableComponent);
	InteractableComponent->StopInteractionGroup();
}

void UInteractionGroup::Select(const uint8 Choice)
{
	if (CurrentNodeType == ECurrentInteractionNodeType::Choice)
	{
		InteractorComponent->InteractionPresenterClass.GetDefaultObject()->OnClearUI(InteractorComponent, InteractableComponent);
		ChoicesSelected.Emplace(Choice);
		NumberOfInteractionNodesTraversed++;
		Iterator = 0;
		OnInteractionTree();
	}
}