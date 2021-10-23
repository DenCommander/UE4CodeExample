// Copyright Epic Games, Inc. All Rights Reserved.

#include "DenSopovExampleHUD.h"
#include "CanvasItem.h"
#include "DSE_GameInstance.h"
#include "HudWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ADenSopovExampleHUD::ADenSopovExampleHUD()
{	
}

void ADenSopovExampleHUD::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwningPawn())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Not found owning pawn. HUD not initialized")));
		}
		return;
	}
	UDSE_GameInstance* GameInstance = Cast<UDSE_GameInstance>(UGameplayStatics::GetGameInstance(GetOwningPawn()));
	if (!GameInstance)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Not found game instance. HUD not initialized")));
		}
		return;
	}

	if (!GameInstance->HudClass)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Not setted HUD class in gameinstance. HUD not initialized")));
		}
		return;
	}
 
	HudWidget = CreateWidget<UHudWidget>(GetOwningPlayerController(), GameInstance->HudClass);	
	HudWidget->AddToViewport(9999);
	HudWidget->UpdateWidget();
}
