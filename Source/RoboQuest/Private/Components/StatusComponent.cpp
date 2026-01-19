// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StatusComponent.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Default initialization
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	ScratchHealth = MaxHealth;
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	ScratchHealth = MaxHealth;
    
	// UI 초기화 등을 위해 방송 <- write in english
	// Broadcast for UI initialization, etc.
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetDamageMultiplier());
	}
}

// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UStatusComponent::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	// 1. Actual health reduction (100% applied)
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	// 2. Scratch Health reduction (reduced by factor, e.g., 50%)
	// Example: Damage 50 * 0.5 = 25 reduction. (100 -> 75)
	float ScratchDamage = DamageAmount * ScratchDamageFactor;
	ScratchHealth = FMath::Clamp(ScratchHealth - ScratchDamage, 0.0f, MaxHealth);
    
	// ScratchHealth cannot be lower than CurrentHealth (correction)
	if (ScratchHealth < CurrentHealth)
	{
		ScratchHealth = CurrentHealth;
	}

	// Notify changes
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetDamageMultiplier());
	}
    
	// Additional logic for death handling, etc...
}

void UStatusComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}

	float RemainingHeal = HealAmount;

	// 1. Calculate the gap to Scratch Health
	float GapToScratch = ScratchHealth - CurrentHealth;

	if (GapToScratch > 0.0f)
	{
		// Heal at 100% efficiency within the Scratch range
		float AmountToHeal = FMath::Min(RemainingHeal, GapToScratch);
		CurrentHealth += AmountToHeal;
		RemainingHeal -= AmountToHeal;
	}

	// 2. Healing beyond Scratch Health (with reduced efficiency)
	if (RemainingHeal > 0.0f)
	{
		float EfficientHeal = RemainingHeal * OverhealEfficiency;
		CurrentHealth += EfficientHeal;
        
		// Since health is full, Scratch Health should also increase accordingly
		ScratchHealth = CurrentHealth;
	}

	// 3. Clamp to not exceed MaxHealth
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
		ScratchHealth = MaxHealth;
	}
    
	// Notify changes
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetDamageMultiplier());
	}
}

float UStatusComponent::GetDamageMultiplier() const
{
	// Level 1 = 1.0, Level 2 = 1.1 ...
	return 1.0f + ((float)(CurrentLevel - 1) * DamageMultiplierPerLevel);
}

