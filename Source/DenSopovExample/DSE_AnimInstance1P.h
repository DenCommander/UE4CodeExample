// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DSE_AnimInstance1P.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DENSOPOVEXAMPLE_API UDSE_AnimInstance1P : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly);
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly);
	bool bIsMoving;
};
