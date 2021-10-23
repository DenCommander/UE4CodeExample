// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DSE_AnimInstance3P.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DENSOPOVEXAMPLE_API UDSE_AnimInstance3P : public UAnimInstance
{
	GENERATED_BODY()

	UFUNCTION()
	void OnLanded();

	UFUNCTION()
	void OnJumped();
	
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void NativeBeginPlay() override;

	UPROPERTY(BlueprintReadOnly);
	bool bIsJumpingNow = false;
	

	UPROPERTY(BlueprintReadOnly);
	float Speed;

	UPROPERTY(BlueprintReadOnly);
	float Direction;
};
