// Copyright Epic Games, Inc. All Rights Reserved.

#include "DenSopovExampleCharacter.h"

#include "DenSopovExampleGameMode.h"
#include "DSE_Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/CharacteristicsComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ADenSopovExampleCharacter

ADenSopovExampleCharacter::ADenSopovExampleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(28.f, 96.0f);	
	GetCapsuleComponent()->SetCollisionProfileName(DefaultCapsuleCollisionProfile);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	//SpringArmComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create a CameraComponent	
	DeathCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera"));
	DeathCameraComponent->SetupAttachment(SpringArmComponent);
	DeathCameraComponent->SetRelativeLocation(FVector(-40.f, 0.f, 0.f)); // Position the camera
	//DeathCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a mesh component that will be used when being viewed from a '3rd person' view (by other players)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetRelativeRotation(FRotator(0.f, -90.0f, 0.f));
	Mesh3P->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	Mesh3P->SetCollisionProfileName(DefaultMeshCollisionProfile);
	
	
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;	
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 78.4f, 9.4f));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComponent->SetIsReplicated(true);

	CharacteristicsComponent = CreateDefaultSubobject<UCharacteristicsComponent>(TEXT("Characteristics"));
	CharacteristicsComponent->SetIsReplicated(true);
}

void ADenSopovExampleCharacter::BeginPlay()
{
	Super::BeginPlay();

	Mesh1P->SetHiddenInGame(false, true);

	if (IsLocallyControlled())
	{		
		FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

		//start periodocal traces to look for interaction actors
		GetWorld()->GetTimerManager().SetTimer(LookForwardTH, this, &ADenSopovExampleCharacter::LookForwardPeriodically, 0.25f, true);
	}
	else
	{		
		FP_Gun->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	MovementModeChangedDelegate.AddDynamic(this,&ADenSopovExampleCharacter::DSE_OnMovementModeChanged);
	
	UCharacteristicsComponent* CharComp = Cast<UCharacteristicsComponent>(GetComponentByClass(UCharacteristicsComponent::StaticClass()));
	CharComp->OnZeroHealth.AddUObject(this,&ADenSopovExampleCharacter::OnDeath);
	CharComp->OnBecomeAlive.AddUObject(this,&ADenSopovExampleCharacter::OnRevive);
}

void ADenSopovExampleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

//////////////////////////////////////////////////////////////////////////
// Jumping
void ADenSopovExampleCharacter::Jump()
{	
	if (CanJump() && !GetCharacterMovement()->IsFalling())
	{
		Super::Jump();
	}
}

void ADenSopovExampleCharacter::DSE_OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode,
    uint8 PreviousCustomMode)
{
	if (GetCharacterMovement()->IsFalling())
	{
		OnJumpedDelegate.Broadcast();
	}
	else
	{
		if (PrevMovementMode == MOVE_Falling)
		{
			OnLandedDelegate.Broadcast();
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
// Input
void ADenSopovExampleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADenSopovExampleCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADenSopovExampleCharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADenSopovExampleCharacter::Local_Fire);
	
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADenSopovExampleCharacter::Local_StartInteraction);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ADenSopovExampleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADenSopovExampleCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADenSopovExampleCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADenSopovExampleCharacter::LookUpAtRate);
}

void ADenSopovExampleCharacter::OnDeath()
{	
	if (IsLocallyControlled() && IsPlayerControlled())
	{
		//if in future there will be bots, we don't need to switch meshes and cameras for bots		
		FirstPersonCameraComponent->SetActive(false);
		DeathCameraComponent->SetActive(true);
		Mesh1P->SetVisibility(false, true);
		Mesh3P->SetOwnerNoSee(false);
	}
	Mesh3P->SetSimulatePhysics(true);
	Mesh3P->SetCollisionProfileName(FName("Ragdoll"));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	GetCharacterMovement()->DisableMovement();

	OnActorDead.Broadcast(this);

	//let gamemode launch logic after player death - like respawning etc.
	auto GameMode = Cast<ADenSopovExampleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnPlayerDead(this);		
	}
}

void ADenSopovExampleCharacter::Revive()
{
	CharacteristicsComponent->AddHealth(CharacteristicsComponent->GetInitialHealth());
	AddAmmo(-1000000,FName("Bomb"));
	AddAmmo(InventoryComponent->InitialAmmoCount,FName("Bomb"));	
}

void ADenSopovExampleCharacter::OnRevive()
{
	if (IsLocallyControlled())
	{
		//if in future there will be bots, we don't need to switch meshes and cameras for bots
		if (IsPlayerControlled())
		{
			FirstPersonCameraComponent->SetActive(true);
			DeathCameraComponent->SetActive(false);
			Mesh1P->SetVisibility(true,true);			
			Mesh3P->SetOwnerNoSee(true);
		}		
	}
	
	Mesh3P->SetSimulatePhysics(false);
	Mesh3P->SetCollisionProfileName(DefaultMeshCollisionProfile);	
	Mesh3P->AttachToComponent(GetCapsuleComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Mesh3P->SetRelativeRotation(FRotator(0.f, -90.0f, 0.f));
	Mesh3P->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	
	GetCapsuleComponent()->SetCollisionProfileName(DefaultCapsuleCollisionProfile);
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);		
}

void ADenSopovExampleCharacter::Local_Fire()
{
	if (!CharacteristicsComponent->IsAlive()) return;
	
	//first check locally if we have bullets, because we call some part of firing logic (like ammocount reducing, SFX and VFX)
	//both on the server and locally on the client
	//- for fast bullets count update, fast interface update, fast reaction to user input (like firing sound etc.)
	//for better player experience

	//at clients we need to rapidly reduce ammocount and refresh interface, for this call part of logic only at client
	//if ammo will reduce only on the server then on the client we can spent more bullets than we have, because of
	//replication delays (right after shot ammo count will be the same until replication come)	
	if (!UKismetSystemLibrary::IsServer(this))
	{
		//on the client we call AddAmmo() to reduce the ammo twice - first on the client side and second on the server in Server_Fire()
		if (!InventoryComponent->AddAmmo_Mutual(-1)) return;
	}
	else
	{
		//if player is the server, then we reduce ammo in Server_Fire, so we don't need to reduce it twice,
		//because both client and server logic runs on the server.
		//Then here we only check if ammo is enough to fire
		if (InventoryComponent->GetAmmoCount()<=0) return;		
	}
			
	//spawning projectiles happens only on the server. Spawntransform calculated on the client, because client-controlled
	//guns (1p view) on the server don't have pitch, so muzzlesocket on the client and on the server has different location:	
	Server_Fire(GetProjSpawnTransform());
	
	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation1P != NULL && IsLocallyControlled())
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation1P, 1.f);
		}
	}
}

FTransform ADenSopovExampleCharacter::GetProjSpawnTransform()
{
	//let's find our target
	FHitResult HitResult;

	FRotator SpawnRotation = GetControlRotation();

	FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();
	
	//some projectiles can be attached to our face/head but they invisible. let's ignore them when we trace for obstacles
	TArray<AActor*> Childrens;
	GetAttachedActors(Childrens);
	
	FCollisionQueryParams Params = FCollisionQueryParams();
	Params.bTraceComplex=false;
	
	if (Childrens.Num()>0)
	{
		Params.AddIgnoredActors(Childrens);
	}
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, GetFirstPersonCameraComponent()->GetComponentLocation(), SpawnLocation, ECollisionChannel::ECC_Visibility,Params))
	{
		//character stands right behind the obstacle. weapon is inside or even behind the wall.
		//So we need to spawn projectiles from actor XY location:		
		FVector TmpVec=GetActorLocation();
		SpawnLocation.X=TmpVec.X;
		SpawnLocation.Y=TmpVec.Y;
	}

	//let's find projectile rotation
	if (GetWorld()->LineTraceSingleByChannel(HitResult, GetFirstPersonCameraComponent()->GetComponentLocation(), GetFirstPersonCameraComponent()->GetComponentLocation()+GetControlRotation().Vector()*1000000, ECollisionChannel::ECC_Visibility, Params))
	{
		SpawnRotation=(HitResult.ImpactPoint-SpawnLocation).Rotation();		
	}

	return FTransform(SpawnRotation,SpawnLocation);
}

void ADenSopovExampleCharacter::Multicast_PlayShotFX_Implementation()
{
	//multicast only for non-owners, because owners played effects locally in 1P
	if (!IsLocallyControlled())
	{
		if (FireAnimation3P != NULL)
		{
			//don't check animinstance for validity, because if it's invalid, the game _must_ crash (someone forgot to make correct setings) 
			Mesh3P->GetAnimInstance()->Montage_Play(FireAnimation3P, 1.f);			
		}

		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void ADenSopovExampleCharacter::Server_Fire_Implementation(FTransform ProjSpawnTransform)
{	
	if (!InventoryComponent->AddAmmo_Mutual(-1)) return;

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// spawn the projectile at the muzzle
			auto Projectile = World->SpawnActorDeferred<ADSE_Projectile>(ProjectileClass, ProjSpawnTransform, nullptr);
			UGameplayStatics::FinishSpawningActor(Projectile,ProjSpawnTransform);
		}
	}
	
	Multicast_PlayShotFX();		
}

bool ADenSopovExampleCharacter::AddAmmo(int InCount, FName Ammo)
{
	if (InventoryComponent)
	{
		return InventoryComponent->AddAmmo_Mutual(InCount, Ammo);
	}
	return false;
}

void ADenSopovExampleCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADenSopovExampleCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADenSopovExampleCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADenSopovExampleCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

AActor* ADenSopovExampleCharacter::LookForward()
{
	if (!CharacteristicsComponent->IsAlive()) return nullptr;
	
	FHitResult TraceHit;		
	
	auto Camera=Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	
	if (Camera)
	{
		FVector CameraLocation = Camera->GetComponentLocation();

		FCollisionQueryParams Params = FCollisionQueryParams();
		Params.TraceTag = TEXT("SphereTraceMulti");
		Params.bReturnFaceIndex = false;
		Params.bReturnPhysicalMaterial = false;
		Params.bTraceComplex = false;

		//some projectiles can be attached to our face/head but they invisible. let's ignore them in interactions
		TArray<AActor*> Childrens;
		GetAttachedActors(Childrens);

		if (Childrens.Num()>0)
		{
			Params.AddIgnoredActors(Childrens);
		}
		
		//making sweep because it's cheeper than trace
		GetWorld()->SweepSingleByChannel(TraceHit, CameraLocation,
            CameraLocation+Camera->GetForwardVector()*LookLength, FQuat::Identity,
            ECC_GameTraceChannel2,
            FCollisionShape::MakeSphere(0.1f), Params);
	}

	auto CurActor = TraceHit.GetActor();
	if (CurActor)
	{
		auto Interactive = Cast<IIInteractiveActor>(CurActor);
		if (Interactive)
		{
			if (CurActor->GetOwner() != static_cast<AActor*>(this))
			{
				Interactive->ActivateInteractWidget();
				return CurActor;
			}
		}
	}		
	
	return nullptr;
}

void ADenSopovExampleCharacter::LookForwardPeriodically()
{
	LookForward();
}

void ADenSopovExampleCharacter::Local_StartInteraction()
{	
	AActor* InteractiveActor = LookForward();
	if (InteractiveActor)
	{
		Server_StartInteraction(InteractiveActor);
	}
	
	if (!UKismetSystemLibrary::IsServer(this))
	{
		Mutual_StartInteraction(InteractiveActor);
	}
}

void ADenSopovExampleCharacter::Server_StartInteraction_Implementation(AActor* InteractiveActor)
{	
	Mutual_StartInteraction(InteractiveActor);
}

void ADenSopovExampleCharacter::Mutual_StartInteraction(AActor* InteractiveActor)
{	
	if (IsValid(InteractiveActor))
	{
		auto InterfaceActor = Cast<IIInteractiveActor>(InteractiveActor);
		if (InterfaceActor)
		{
			InterfaceActor->Interact(this);
		}		
	}
}
