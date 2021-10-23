// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacteristicsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FDelegateNoParam);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DENSOPOVEXAMPLE_API UCharacteristicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacteristicsComponent();

	FDelegateNoParam OnCharacteristicsChanged;

	FDelegateNoParam OnZeroHealth;

	FDelegateNoParam OnBecomeAlive;

	FDelegateNoParam OnComponentDestroyedDelegate;
	
	static float GetHealth(AActor* InActor);

	float GetHealth();	

	//supposed to run only on the server side
	static void AddHealth(AActor* InActor, float InHealth);

	//supposed to run only on the server side
	void AddHealth(float InHealth);	

	//supposed to run only on the server side
	void SetHealth(float InHealth);

	bool IsAlive();

	float GetInitialHealth();
	
protected:
	void InitializeCharacteristics();
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bIsAlive=true;

	UFUNCTION()
	void TakeDamage( AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy,
                                                AActor* DamageCauser );
	
	UPROPERTY(EditDefaultsOnly)
	float InitialHealth=100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UFUNCTION()
	void OnRep_Health();
};
