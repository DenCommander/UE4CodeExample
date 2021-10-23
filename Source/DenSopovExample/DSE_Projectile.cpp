// Copyright Epic Games, Inc. All Rights Reserved.

#include "DSE_Projectile.h"

#include "DenSopovExampleCharacter.h"
#include "CommonStructs.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ADSE_Projectile::ADSE_Projectile():
TimeToExplodeIfHitted(4.f),TimeToExplodeIfMissed(8.f),BlinkingTime(3.f) 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADSE_Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(CollisionComp);		
	InteractWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	InitialLifeSpan = TimeToExplodeIfMissed+0.5f;	
}

void ADSE_Projectile::ActivateInteractWidget()
{	
	InteractWidget->SetHiddenInGame(false);
		 
	 GetWorld()->GetTimerManager().SetTimer(WidgetVisibilityOffTH, this, &ADSE_Projectile::DeactivateInteractWidget,
                                                0.6f, false);
}

void ADSE_Projectile::DeactivateInteractWidget()
{	
	InteractWidget->SetHiddenInGame(true);	
}

//interact with projectile to take it. called both on the server and locally
void ADSE_Projectile::Interact(AActor* InteractActor)
{
	ADenSopovExampleCharacter* Char = Cast<ADenSopovExampleCharacter>(InteractActor);
	if (Char)
	{			
		Char->AddAmmo(1, AmmoType.RowName);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	
	Destroy();	
}

void ADSE_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(TimeToExplodeIfMissed+0.5f);
	
	if (!AmmoType.DataTable || AmmoType.RowName==NAME_None || AmmoType.DataTable->RowStruct != FAmmoTypes::StaticStruct())
	{
		if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
						FString::Printf(TEXT("Projectile not initialized correctly. Ammotype is invalid")));
				}
	}
	
	if (TimeToExplodeIfHitted>TimeToExplodeIfMissed)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
        FString::Printf(TEXT("TimeToExplodeIfHitted should be less than TimeToExplodeIfMissed!")));
		}
	}

	CurBlinkFreq = MinBlinkFreq;

	if (BlinkingTime >= TimeToExplodeIfMissed || BlinkingTime >= TimeToExplodeIfHitted)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
        FString::Printf(TEXT("BlinkingTime should be less than TimeToExplode!")));
		}

		EnableBlinking();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(BlinkingTH, this, &ADSE_Projectile::EnableBlinking,
												TimeToExplodeIfMissed - BlinkingTime, false);
	}

	GetWorld()->GetTimerManager().SetTimer(ExplodeTH, this, &ADSE_Projectile::Explode, TimeToExplodeIfMissed, false);

	//prepare material for blinking
	UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
	UMaterialInterface* MaterialInterface = StaticMeshComponent->GetMaterial(0);
	if (MaterialInterface)
	{
		ProjMatInstance = StaticMeshComponent->CreateDynamicMaterialInstance(0,MaterialInterface);
		StaticMeshComponent->SetMaterial(0,ProjMatInstance);
	}

	InteractWidget->SetHiddenInGame(true);
}

float ADSE_Projectile::GetCurrentProjColorIndex(float DeltaTime)
{	
	CurBlinkFreq = FMath::FInterpConstantTo(CurBlinkFreq, MaxBlinkFreq, DeltaTime, FreqInterpSpeed);
	CurrentAngle += DeltaTime * CurBlinkFreq;
	if (FMath::Sin(CurrentAngle * 2.f * 3.141592f) > 0)
	{
		return 1.f;
	}
	else
	{
		return 0.f;
	}
}

void ADSE_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBlinkingEnabled)
	{
		
		ProjMatInstance->SetScalarParameterValue(FName("ColorIndex"), GetCurrentProjColorIndex(DeltaTime));
	}
}

void ADSE_Projectile::Destroyed()
{
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, ExplodeTH);
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, WidgetVisibilityOffTH);
	UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, BlinkingTH);	
}

void ADSE_Projectile::EnableBlinking()
{	
	bIsBlinkingEnabled=true;	
}


void ADSE_Projectile::Server_Explode_Implementation()
{		
	FTransform SpawnTransform(GetActorTransform());
	SpawnTransform.SetScale3D(FVector(2.f,2.f,2.f));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeParticles, SpawnTransform, true);
	
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());	
}

void ADSE_Projectile::Explode()
{		
	FTransform SpawnTransform(GetActorTransform());
	SpawnTransform.SetScale3D(FVector(2.f,2.f,2.f));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeParticles, SpawnTransform, true);
	
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	
	SetActorHiddenInGame(true);
	
	if (UKismetSystemLibrary::IsServer(this))
	{
		TArray<AActor*> IgnoredActors;

		UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 0.f,
			SpawnTransform.GetLocation(), DamageRadius*0.5f,DamageRadius,
			1.0f, UDamageType::StaticClass(),	IgnoredActors, this, nullptr,
			ECC_Visibility);

		SetLifeSpan(0.5f);
	}
}

void ADSE_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor) && OtherActor->GetClass()->IsChildOf<ADenSopovExampleCharacter>())
	{		
		AttachToActor(OtherActor,FAttachmentTransformRules::KeepWorldTransform);

		//let's make that the player with the bomb attached will not see it
		//(because the bomb is hanging in the air, because the player does not see his body)
		SetOwner(OtherActor);
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetOwnerNoSee(true);
		}
		
		GetProjectileMovement()->Deactivate();
		
		GetWorld()->GetTimerManager().SetTimer(ExplodeTH, this, &ADSE_Projectile::Explode, TimeToExplodeIfHitted, false);
		GetWorld()->GetTimerManager().SetTimer(BlinkingTH, this, &ADSE_Projectile::EnableBlinking, TimeToExplodeIfHitted-BlinkingTime, false);		
	}
}
