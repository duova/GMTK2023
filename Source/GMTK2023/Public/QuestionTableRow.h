// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestionTableRow.generated.h"

USTRUCT(BlueprintType)
struct GMTK2023_API FQuestionTableRow : public FTableRowBase
{
	GENERATED_BODY()
	
	FQuestionTableRow();

	UPROPERTY()
	FText Question;

	UPROPERTY()
	FText Answer;

	UPROPERTY()
	float Value;
};
