// Fill out your copyright notice in the Description page of Project Settings.


#include "HudWidget.h"

#include "CommonStructs.h"
#include "Components/CharacteristicsComponent.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "SOMESPACE"

void UHudWidget::Deinitialize()
{
	bInitialized = false;
}

void UHudWidget::Deinitialize(AActor* InActor)
{
	Deinitialize();
}

void UHudWidget::UpdateWidget()
{
	// all the checks for nullptr is at the nativeconstruct().	
	//also bInitialized will become false in case of destroying any component (characteristics or inventory) or owner pawn
	//as we binded deinitialize() func to those events at nativeconstruct() here
	//this tricks allows us to make less checkings at updatewidget() and to have slightly cheaper and more readable code
	if (!bInitialized) return;

	auto CurAmmoType = GetCashedInventory()->GetCurrentAmmoName();
	
	UDataTable* AmmoTypesTable = GetCashedGameInstance()->AmmoTypesTable;
	//table checked for existance and correct structure in GameInstance::Init()
	if (AmmoTypesTable->GetRowNames().Contains(CurAmmoType))
	{
		FAmmoTypes* AmmoData = AmmoTypesTable->FindRow<FAmmoTypes>(CurAmmoType, "");
		
		AmmoIcon->SetBrush(AmmoData->Icon);				
	}	
	
	AmmoCount->SetText(FText::Format(LOCTEXT("AmmoText", "{0}"), GetCashedInventory()->GetAmmoCount(CurAmmoType)));		
	HealthCount->SetText(FText::Format(LOCTEXT("HealthText", "{0}"), GetCashedCharacteristics()->GetHealth()));		
}

UDSE_GameInstance* UHudWidget::GetCashedGameInstance()
{
	if (GameInstance) return GameInstance;
	
	GameInstance = Cast<UDSE_GameInstance>(UGameplayStatics::GetGameInstance(GetCashedPawn()));
	return GameInstance;
}

APawn* UHudWidget::GetCashedPawn()
{
	if (CashedPawn) return CashedPawn;
	CashedPawn=GetOwningPlayerPawn();
	return CashedPawn;
}

UInventoryComponent* UHudWidget::GetCashedInventory()
{
	if (CashedInventory) return CashedInventory;
	
	if (GetCashedPawn())
	{
		CashedInventory = Cast<UInventoryComponent>(GetCashedPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));	
	}	
	return CashedInventory;
}

UCharacteristicsComponent* UHudWidget::GetCashedCharacteristics()
{
	if (CashedCharacteristics) return CashedCharacteristics;
	
	if (GetCashedPawn())
	{
		CashedCharacteristics = Cast<UCharacteristicsComponent>(GetCashedPawn()->GetComponentByClass(UCharacteristicsComponent::StaticClass()));	
	}	
	return CashedCharacteristics;
}

void UHudWidget::NativeConstruct()
{
	if (!GetCashedPawn() || !GetCashedInventory() || !GetCashedCharacteristics() && HealthCount && AmmoCount)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot initialize HUD"));
		if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
						FString::Printf(TEXT("Cannot initialize HUD")));
				}
		return;
	}
	
	UCharacteristicsComponent* CharComp = Cast<UCharacteristicsComponent>(GetCashedPawn()->GetComponentByClass(UCharacteristicsComponent::StaticClass()));
	if (!CharComp) return; 
	
	CharComp->OnCharacteristicsChanged.AddUObject(this,&UHudWidget::UpdateWidget);

	GetCashedInventory()->OnAmmoChanged.AddUObject(this,&UHudWidget::UpdateWidget);

	GetCashedInventory()->OnComponentDestroyedDelegate.AddUObject(this,&UHudWidget::Deinitialize);
	GetCashedCharacteristics()->OnComponentDestroyedDelegate.AddUObject(this,&UHudWidget::Deinitialize);
	GetCashedPawn()->OnDestroyed.AddDynamic(this,&UHudWidget::Deinitialize);

	bInitialized = true;
}




	
