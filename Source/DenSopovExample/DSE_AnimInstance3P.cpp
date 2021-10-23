// Fill out your copyright notice in the Description page of Project Settings.


#include "DSE_AnimInstance3P.h"

#include "DenSopovExampleCharacter.h"

void UDSE_AnimInstance3P::OnLanded()
{
	bIsJumpingNow=false;
}

void UDSE_AnimInstance3P::OnJumped()
{	
	bIsJumpingNow=true;
}


void UDSE_AnimInstance3P::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	auto OwnerCharacter = TryGetPawnOwner();
	if (!OwnerCharacter) return;

	FVector CurVelocity = OwnerCharacter->GetVelocity();
	
	Direction = CalculateDirection(CurVelocity,OwnerCharacter->GetActorRotation());
	Speed = CurVelocity.Size2D();
}

void UDSE_AnimInstance3P::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	ADenSopovExampleCharacter* OwnerCharacter = Cast<ADenSopovExampleCharacter>(TryGetPawnOwner());
	if (!OwnerCharacter) return;	
	
	OwnerCharacter->OnLandedDelegate.AddDynamic(this,&UDSE_AnimInstance3P::OnLanded);
	OwnerCharacter->OnJumpedDelegate.AddDynamic(this,&UDSE_AnimInstance3P::OnJumped);
}
