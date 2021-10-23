// Copyright Epic Games, Inc. All Rights Reserved.

#include "DenSopovExampleGameMode.h"
#include "DenSopovExampleHUD.h"
#include "DenSopovExampleCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADenSopovExampleGameMode::ADenSopovExampleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADenSopovExampleHUD::StaticClass();
}

void ADenSopovExampleGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ADenSopovExampleGameMode::RevivePlayer(ADenSopovExampleCharacter* InCharacter)
{
	InCharacter->Revive();
}

void ADenSopovExampleGameMode::OnPlayerDead(ADenSopovExampleCharacter* InCharacter)
{
	FTimerHandle RespawnTimer;
	FTimerDelegate TimerDelegate;	
	TimerDelegate.BindUFunction(this, FName("RevivePlayer"), InCharacter);	
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, TimerDelegate, RespawnTime, false);
}