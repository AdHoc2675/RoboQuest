// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatusComponent.h"
#include "Data/EnemyStatRow.h"
#include "Engine/DataTable.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Default initialization
	CurrentHealth = MaxHealth;
	ScratchHealth = MaxHealth;
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	ScratchHealth = MaxHealth;
    
	// Initial Health broadcast
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, ScratchHealth, MaxHealth);
	}

	// Initial EXP broadcast
	if (OnExpChanged.IsBound())
	{
		OnExpChanged.Broadcast(CurrentExp, MaxExp, CurrentLevel);
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

	// Actual health reduction (100% applied)
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	// Scratch Health reduction (reduced by factor, e.g., 50%)
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
		OnHealthChanged.Broadcast(CurrentHealth, ScratchHealth, MaxHealth);
	}
    
	UE_LOG(LogTemp, Log, TEXT("UStatusComponent:: %s Took Damage: %f, CurrentHealth: %f, ScratchHealth: %f"), *GetOwner()->GetName(), DamageAmount, CurrentHealth, ScratchHealth);

	// Additional logic for death handling, etc...
}

void UStatusComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}

	float RemainingHeal = HealAmount;

	// Calculate the gap to Scratch Health
	float GapToScratch = ScratchHealth - CurrentHealth;

	if (GapToScratch > 0.0f)
	{
		// Heal at 100% efficiency within the Scratch range
		float AmountToHeal = FMath::Min(RemainingHeal, GapToScratch);
		CurrentHealth += AmountToHeal;
		RemainingHeal -= AmountToHeal;
	}

	// Healing beyond Scratch Health (with reduced efficiency)
	if (RemainingHeal > 0.0f)
	{
		float EfficientHeal = RemainingHeal * OverhealEfficiency;
		CurrentHealth += EfficientHeal;
        
		// Since health is full, Scratch Health should also increase accordingly
		ScratchHealth = CurrentHealth;
	}

	// Clamp to not exceed MaxHealth
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth;
		ScratchHealth = MaxHealth;
	}
    
	// Notify changes
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, ScratchHealth, MaxHealth);
	}
}

float UStatusComponent::GetDamageMultiplier() const
{
	// Level 1 = 1.0, Level 2 = 1.1 ...
	return 1.0f + ((float)(CurrentLevel - 1) * DamageMultiplierPerLevel);
}

void UStatusComponent::AddExp(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	CurrentExp += Amount;

	// Check for level up
	while (CurrentExp >= MaxExp)
	{
		CurrentExp -= MaxExp;
		CurrentLevel++;

		// Update MaxExp for next level
		UpdateNextLevelExp();

		// Broadcast level up event
		if (OnLevelUp.IsBound())
		{
			OnLevelUp.Broadcast(CurrentLevel);
		}
	}
	// Broadcast EXP change
	if (OnExpChanged.IsBound())
	{
		OnExpChanged.Broadcast(CurrentExp, MaxExp, CurrentLevel);
	}

	// Notify changes
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, ScratchHealth, MaxHealth);
	}
}

void UStatusComponent::UpdateNextLevelExp()
{
	MaxExp = MaxExp * ExpIncreaseFactor;
	MaxHealth = MaxHealth + 40;
	ScratchHealth = ScratchHealth + 40;
	CurrentHealth = CurrentHealth + 40;


}

// --- Enemy Stat ---
void UStatusComponent::InitializeEnemyStats(FName EnemyRowName, int32 NewLevel)
{
	if (!EnemyStatDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyStatDataTable is not assigned!"));
		return;
	}

	// Find the row in the data table
	FEnemyStatRow* EnemyStats = EnemyStatDataTable->FindRow<FEnemyStatRow>(EnemyRowName, TEXT(""));

	if (EnemyStats)
	{
		CurrentLevel = NewLevel;

		// Apply level scaling
		float LevelScale = FMath::Pow(LevelScalingFactor, CurrentLevel - 1);

		MaxHealth = EnemyStats->BaseHealth * LevelScale;
		CurrentHealth = MaxHealth;
		ScratchHealth = MaxHealth;

		// You can also initialize other stats like damage, experience, etc.
		ExpReward = EnemyStats->ExpReward * LevelScale;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyStatRow not found for %s"), *EnemyRowName.ToString());
	}
}