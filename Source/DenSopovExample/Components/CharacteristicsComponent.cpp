
#include "CharacteristicsComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCharacteristicsComponent::UCharacteristicsComponent() : Health(InitialHealth)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// ...
}

// Called when the game starts
void UCharacteristicsComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()) return;
	GetOwner()->OnTakeRadialDamage.AddDynamic(this, &UCharacteristicsComponent::TakeDamage);
	InitializeCharacteristics();
}

void UCharacteristicsComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	OnComponentDestroyedDelegate.Broadcast();
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UCharacteristicsComponent::TakeDamage( AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy,
											AActor* DamageCauser )
{
	AddHealth(-Damage);
}

UCharacteristicsComponent* GetCharacteristicsComponent(AActor* InActor)
{
	if (!InActor) return nullptr;
	
	return Cast<UCharacteristicsComponent>(InActor->GetComponentByClass(UCharacteristicsComponent::StaticClass()));
}

float UCharacteristicsComponent::GetHealth(AActor* InActor)
{
	float result = 0.f;
	
	if (!IsValid(InActor)) return result;
	
	UCharacteristicsComponent* CharComp = Cast<UCharacteristicsComponent>(InActor->GetComponentByClass(UCharacteristicsComponent::StaticClass()));\
	if (!CharComp) return result; 
	
	result = CharComp->GetHealth();
	
	return result;
}

void UCharacteristicsComponent::AddHealth(float InHealth)
{
	if (FMath::IsNearlyZero(InHealth)) return;
	
	float result = GetHealth()+InHealth;
	result=FMath::Max(result,0.f);
	
	SetHealth(result);
}

void UCharacteristicsComponent::AddHealth(AActor* InActor, float InHealth)
{
	if (!IsValid(InActor)) return;
	
	UCharacteristicsComponent* CharComp = Cast<UCharacteristicsComponent>(InActor->GetComponentByClass(UCharacteristicsComponent::StaticClass()));
	if (!CharComp) return; 
	
	CharComp->AddHealth(InHealth);	
}

float UCharacteristicsComponent::GetHealth()
{
	return Health;
}

void UCharacteristicsComponent::OnRep_Health()
{
	OnCharacteristicsChanged.Broadcast();
	if (FMath::IsNearlyZero(Health))
	{
		bIsAlive = false;
		OnZeroHealth.Broadcast();
	}
	else
	{
		if (!bIsAlive && Health>0.f)
		{
			bIsAlive = true;
			OnBecomeAlive.Broadcast();
		}
	}	
}

void UCharacteristicsComponent::SetHealth(float InHealth)
{
	if (GetHealth()!=InHealth)
	{
		Health=InHealth;
		OnRep_Health();
	}	
}

bool UCharacteristicsComponent::IsAlive()
{
	return bIsAlive;
}

float UCharacteristicsComponent::GetInitialHealth()
{
	return InitialHealth;
}

void UCharacteristicsComponent::InitializeCharacteristics()
{
	SetHealth(InitialHealth);
}

void UCharacteristicsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacteristicsComponent, Health);	
}
