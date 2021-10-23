// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FDelegateNoParam);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DENSOPOVEXAMPLE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	int GetAmmoCount(FName InAmmoName = NAME_None);
	int GetAmmoSlotByName(FName Ammo);

	
	/** non-RPC function. Can be called both on the Client and Server (on the client calls for fast reaction to player input)
	 *it can add ammo to any slot if InAmmoType explicitly defined, else it adds ammo for the current used weapon slot
	* @param InCount - Count of bullets to add.
	* @param InAmmoType - Name of ammo type to add. If not defined, then ammo adds to current weapon slot	
	* @return return true if ammo successfully added
	*/
	bool AddAmmo_Mutual(int InCount, FName InAmmoType = NAME_None);

	FDelegateNoParam OnAmmoChanged;

	FDelegateNoParam OnComponentDestroyedDelegate;

	UPROPERTY(EditDefaultsOnly)
	int InitialAmmoCount=3;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Ammo count array. Any element of array is corresponds to the same element of AmmoTypesArray
	UPROPERTY(ReplicatedUsing=OnRep_AmmoArray)
	TArray<int> AmmoArray;

	UFUNCTION()
	void OnRep_AmmoArray();

	UPROPERTY(Replicated)
	TArray<FName> AmmoTypesArray;
	
	int CurrentAmmoSlot = -1;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	FName GetCurrentAmmoName() const
	{
		if (AmmoTypesArray.Num()-1>=GetCurrentAmmoSlot())
		{
			return AmmoTypesArray[GetCurrentAmmoSlot()];
		}
		return FName("");
	}
	
	int GetCurrentAmmoSlot() const
	{
		return CurrentAmmoSlot;
	}

	void SetCurrentAmmoSlot(int inAmmoSlot)
	{
		this->CurrentAmmoSlot = inAmmoSlot;
	}

	//now not used because we have no different weapons yet
	UFUNCTION(Server,Reliable)
	void Server_SetCurrentAmmoSlot(int inAmmoSlot);	
};
