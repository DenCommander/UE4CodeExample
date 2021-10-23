// Fill out your copyright notice in the Description page of Project Settings.


#include "DSE_AnimInstance1P.h"
#include "DenSopovExampleCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UDSE_AnimInstance1P::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto OwnerCharacter = TryGetPawnOwner();
	if (!OwnerCharacter) return;

	bIsInAir = OwnerCharacter->GetMovementComponent()->IsFalling();
	
	FVector CurVelocity = OwnerCharacter->GetVelocity();

	bIsMoving = OwnerCharacter->GetVelocity().Size()>0.f;	
}

