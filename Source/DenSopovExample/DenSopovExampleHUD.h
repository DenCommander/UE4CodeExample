// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "HudWidget.h"
#include "GameFramework/HUD.h"
#include "DenSopovExampleHUD.generated.h"

UCLASS()
class ADenSopovExampleHUD : public AHUD
{
	GENERATED_BODY()

public:
	ADenSopovExampleHUD();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UHudWidget* HudWidget;
};

