// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractorComponent.generated.h"

class UInteractionPresenter;
class UInteractorComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateDirty, UInteractorComponent*);

/**
 * Component that should be on a player actor that allows for "interaction".
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INTERACTABLESYSTEM_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UInteractableComponent;

public:
	UInteractorComponent();

	UFUNCTION(BlueprintCallable)
	void Continue() const;

	UFUNCTION(BlueprintCallable)
	void Leave() const;

	UFUNCTION(BlueprintCallable)
	void Select0() const;

	UFUNCTION(BlueprintCallable)
	void Select1() const;

	UFUNCTION(BlueprintCallable)
	void Select2() const;

	UFUNCTION(BlueprintCallable)
	void Select3() const;

	UFUNCTION(BlueprintCallable)
	void Select4() const;

	UFUNCTION(BlueprintCallable)
	void Select5() const;

	UFUNCTION(BlueprintCallable)
	void StateDirty();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInteractionPresenter> InteractionPresenterClass;
	
	FOnStateDirty OnStateDirtyDelegate;

private:
	
	UPROPERTY()
	UInteractableComponent* LastInteraction;
};
