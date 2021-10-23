// Fill out your copyright notice in the Description page of Project Settings.


#include "DSE_GameInstance.h"

#include "CommonStructs.h"

void UDSE_GameInstance::Init()
{
	Super::Init();
	if(!AmmoTypesTable || !AmmoTypesTable->RowStruct || AmmoTypesTable->RowStruct != FAmmoTypes::StaticStruct())
	{
		UE_LOG(LogTemp, Error, TEXT("AmmoTypesTable not setted up properly!"));		
	}
}
