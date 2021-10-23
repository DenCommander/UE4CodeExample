// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

int UInventoryComponent::GetAmmoCount(FName InAmmoName)
{
	int CurSlot;
	if (InAmmoName==NAME_None)
	{
		CurSlot=GetCurrentAmmoSlot();
	}
	else
	{
		CurSlot=GetAmmoSlotByName(InAmmoName);
	}

	if (CurSlot>=0 && AmmoArray.Num()>CurSlot)
	{
		return AmmoArray[CurSlot];
	}
	
	return 0;
}

int UInventoryComponent::GetAmmoSlotByName(FName Ammo)
{
	for (int count = 0; count < AmmoTypesArray.Num(); ++count)
	{
		if (AmmoTypesArray[count] == Ammo)
		{
			return count;
		}
	}
	return -1;
}

bool UInventoryComponent::AddAmmo_Mutual(int InCount, FName InAmmoType)
{
	if (InCount==0)
	{
		return false;
	}

	int AmmoSlot;
	if (InAmmoType == NAME_None)
	{
		AmmoSlot=GetCurrentAmmoSlot();
		
		//we are trying to add ammo to the current slot, but the current slot is undefinded. In this case need to call
		//function with explicit parameter InAmmoType
		if (AmmoSlot<0)
		{
			return false;
		}
	}
	else
	{
		AmmoSlot = GetAmmoSlotByName(InAmmoType);
	}	 

	int NewAmmoCount;

	if (InCount < 0)		
	{
		//we have no slots with this ammo type, so we can't to decrease ammo
		if (AmmoSlot<0)
		{
			return false;
		}			

		int AmmoToDecrease = FMath::Clamp(-InCount,0,AmmoArray[AmmoSlot]);
		
		if (AmmoToDecrease==0) return false;
		NewAmmoCount = AmmoArray[AmmoSlot] - AmmoToDecrease;
	}
	else
	{
		if (AmmoSlot<0)
		{
			AmmoTypesArray.Add(InAmmoType);
			AmmoArray.Add(0.f);
			AmmoSlot = AmmoArray.Num()-1;
		}
		NewAmmoCount = AmmoArray[AmmoSlot]+InCount;
	}		
	
	AmmoArray[AmmoSlot]=NewAmmoCount;
	OnRep_AmmoArray();
	return true;		
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// A little bit of hardcode. In real game better to have data-driven initialization
	if (UKismetSystemLibrary::IsServer(this))
	{
		AddAmmo_Mutual(InitialAmmoCount, FName("Bomb"));
	}		
}

void UInventoryComponent::OnRep_AmmoArray()
{
	if (GetCurrentAmmoSlot()<0 && AmmoArray.Num()>0)
	{
		SetCurrentAmmoSlot(0);
	}
	OnAmmoChanged.Broadcast();
}

void UInventoryComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	OnComponentDestroyedDelegate.Broadcast();
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, AmmoTypesArray);
	DOREPLIFETIME(UInventoryComponent, AmmoArray);	
}

void UInventoryComponent::Server_SetCurrentAmmoSlot_Implementation(int inAmmoSlot)
{
	SetCurrentAmmoSlot(inAmmoSlot);
}
