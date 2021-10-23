// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IInteractiveActor.h"

#include "DSE_Projectile.generated.h"

UCLASS(config=Game)
class ADSE_Projectile : public AActor, public IIInteractiveActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidget;
				
	FTimerHandle ExplodeTH;

	FTimerHandle WidgetVisibilityOffTH;

	/** Time before the explode if projectile attached to pawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float TimeToExplodeIfHitted;

	/** Time before the explode if projectile missed to pawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float TimeToExplodeIfMissed;
	
	/** how many seconds the projectile will blink before explode */
	UPROPERTY(EditDefaultsOnly, Category=Blinking)
	float BlinkingTime;	

	//maximum frequency of projectile blinking before explode
	UPROPERTY(EditDefaultsOnly, Category=Blinking)
	float MaxBlinkFreq=5.f;
	
	//how fast will blinking frequency increase (speed of interpolation in command "finterpto")
	UPROPERTY(EditDefaultsOnly, Category=Blinking)
	float FreqInterpSpeed=2.f;
	
	//blinking frequency, at the start
	UPROPERTY(EditDefaultsOnly, Category=Blinking)
	float MinBlinkFreq=1.f;

	//current blinking frequency, at the moment
	float CurBlinkFreq=0.f;
	
	//we use sin() function to switch colors of material. CurrentAngle is current position passed to the sin()
	float CurrentAngle=0.0001f;

	FTimerHandle BlinkingTH;
	bool bIsBlinkingEnabled=false;

	UPROPERTY()
	UMaterialInstanceDynamic* ProjMatInstance;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	UParticleSystem* ExplodeParticles;
	
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	USoundBase* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float Damage=20.f;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float DamageRadius=200.f;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	FDataTableRowHandle AmmoType;

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	USoundBase* PickupSound;

	void Explode();
	
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

public:
	ADSE_Projectile();

	//-----=====interface IIInteractiveActor start
	virtual void ActivateInteractWidget() override;

	//interact with projectile to take it. called both on the server and locally
	virtual void Interact(AActor* InteractActor) override;

	//-----=====interface IIInteractiveActor end

	void DeactivateInteractWidget();
	
	UFUNCTION(Server, Unreliable)
    void Server_Explode();
	
protected:
	virtual void BeginPlay() override;	
	
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	void EnableBlinking();	

	//calculates color index to pass to material and switch colors of projectile (for blinking)
	float GetCurrentProjColorIndex(float DeltaTime);	
};

