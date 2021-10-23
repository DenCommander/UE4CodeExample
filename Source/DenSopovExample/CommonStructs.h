// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Engine/DataTable.h"
#include "CommonStructs.generated.h"

USTRUCT(BlueprintType)
struct FAmmoTypes : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText AmmoType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateBrush Icon;	
};