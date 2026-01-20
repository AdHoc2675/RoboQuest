// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

// Health change event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, ScratchHealth, float, MaxHealth);
// Experience change event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExpChanged, float, CurrentExp, float, NextLevelExp, int32, CurrentLevel);
// Level up event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOQUEST_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Data table to use (assigned in editor)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|Data")
	class UDataTable* EnemyStatDataTable;

	// --- Health ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHealth = 180.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float CurrentHealth;

	// potential health that can be healed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float ScratchHealth;
    
    // The rate at which Scratch Health decreases when taking damage. (0.5 = half the damage)
    // Example: 50 damage -> Health -50, Scratch -25
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Config")
    float ScratchDamageFactor = 0.5f;

    // The efficiency applied when healing beyond the Scratch Health value. (0.1 = 10% efficiency)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Config")
    float OverhealEfficiency = 0.1f;

	// --- Leveling & EXP ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 CurrentLevel = 1;

	// Multiply = 1.0 + (Level - 1) * DamageMultiplierPerLevel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float DamageMultiplierPerLevel = 0.1f;
 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Experience")
	float CurrentExp = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Experience")
	float MaxExp = 100.0f;

	// Experience increase factor per level (e.g., 1.2 means 20% more EXP required each level)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Experience")
	float ExpIncreaseFactor = 1.2f;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	// Experience change event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnExpChanged OnExpChanged;

	// Level up event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelUp OnLevelUp;

	// --- Functions ---
	UFUNCTION(BlueprintCallable, Category = "Status")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void Heal(float HealAmount);

	// calculated as 1.0 + (CurrentLevel - 1) * DamageMultiplierPerLevel
	UFUNCTION(BlueprintPure, Category = "Status")
	float GetDamageMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Status")
	void AddExp(float Amount);

	// Update NextLevelExp based on CurrentLevel and ExpIncreaseFactor
	void UpdateNextLevelExp();

	// --- Enemy Stat Initialization ---
	// Initialization function: Set stats based on enemy ID (RowName) and level
	UFUNCTION(BlueprintCallable, Category = "Status")
	void InitializeEnemyStats(FName EnemyRowName, int32 NewLevel);

	// Stat increase rate per level (e.g., 1.1 means 10% increase per level)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status|Config")
	float LevelScalingFactor = 1.1f;
};
