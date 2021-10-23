// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DSE_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"


#include "HudWidget.generated.h"

class UCharacteristicsComponent;
/**
 * 
 */
UCLASS()
class DENSOPOVEXAMPLE_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* AmmoIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoCount;

	UPROPERTY(meta = (BindWidget))
	UImage* HealthIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthCount;

	UPROPERTY()
	UDSE_GameInstance* GameInstance;
	
	UDSE_GameInstance* GetCashedGameInstance();

	UPROPERTY()
	APawn* CashedPawn;
	
	//cash pawn for fast often access
	APawn* GetCashedPawn();

	UPROPERTY()
	UInventoryComponent* CashedInventory;

	UPROPERTY()
	UCharacteristicsComponent* CashedCharacteristics;
	
	//cash inventory for fast often access
	UInventoryComponent* GetCashedInventory();

	//cash characteristics for fast often access
	UCharacteristicsComponent* GetCashedCharacteristics();
	
	virtual void NativeConstruct() override;

	//to avoid checkings all the time in update
	bool bInitialized = false;

	void Deinitialize();

	UFUNCTION()
	void Deinitialize(AActor* InActor);

public:
	void UpdateWidget();
};
