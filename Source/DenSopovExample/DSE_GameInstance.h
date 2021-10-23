// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "DSE_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DENSOPOVEXAMPLE_API UDSE_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDataTable* AmmoTypesTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> HudClass;

protected:
	// Called when the game starts or when spawned
	virtual void Init() override;
};
