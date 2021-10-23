// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DenSopovExampleCharacter.generated.h"

class UInputComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLandedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterDead, class ADenSopovExampleCharacter*);

UCLASS(config=Game)
class ADenSopovExampleCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Pawn mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* DeathCameraComponent;
		
	UPROPERTY(VisibleDefaultsOnly)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UCharacteristicsComponent* CharacteristicsComponent;

	//-----=====looking for interact actors part start:

	UPROPERTY(EditDefaultsOnly)
	float LookLength=300.f;

	AActor* LookForward();

	void LookForwardPeriodically();	

	FTimerHandle LookForwardTH;

	//-----=====looking for interact actors part end

	UFUNCTION(Server,Unreliable)
    void Server_StartInteraction(AActor* InteractiveActor);

	//interaction logic calls both on the client and on the server for fast response to user input
	void Mutual_StartInteraction(AActor* InteractiveActor);

	void Local_StartInteraction();

	void Jump();	
	
public:
	ADenSopovExampleCharacter();
	
	//delegate to inform animation blueprint about character's jump
	UPROPERTY(BlueprintAssignable, Category=Gameplay)
	FOnJumpedDelegate OnJumpedDelegate;
	
	//delegate to inform animation blueprint about character's landing
	UPROPERTY(BlueprintAssignable, Category=Gameplay)
	FOnLandedDelegate OnLandedDelegate;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ADSE_Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation1P;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation3P;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	FName DefaultCapsuleCollisionProfile = FName("Pawn");

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	FName DefaultMeshCollisionProfile = FName("CharacterMesh");

	FOnCharacterDead OnActorDead;

	bool AddAmmo(int InCount, FName Ammo = NAME_None);

	void Revive();	

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime);

	UFUNCTION()
    void DSE_OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	/* called when revive started*/
	void OnRevive();
	
	/** Fires a projectile. */
	UFUNCTION( Server, Reliable )
	void Server_Fire(FTransform ProjSpawnTransform);

	//onrep health goes to all connections, then Characteristics comp call delegate onzerohealth, then actor
	//launch OnDeath() on the all connections (because actor binded to onzerohealth event)
	void OnDeath();	

	//initial call of fire on the client
	void Local_Fire();

	//calculates start position and rotation of projectile
	FTransform GetProjSpawnTransform();

	//used for multicasr effects of weapon's shot
	UFUNCTION(NetMulticast,Unreliable)
	void Multicast_PlayShotFX();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface	

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

