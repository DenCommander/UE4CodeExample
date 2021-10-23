// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//#include "DenSopovExampleCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "DenSopovExampleGameMode.generated.h"

UCLASS(minimalapi)
class ADenSopovExampleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADenSopovExampleGameMode();

	void OnPlayerDead(class ADenSopovExampleCharacter* InCharacter);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void RevivePlayer(class ADenSopovExampleCharacter* InCharacter);	

	UPROPERTY(EditDefaultsOnly)
	float RespawnTime=10.f;
};



