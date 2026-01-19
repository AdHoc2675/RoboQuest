// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, DamageMultiplier);

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

	// --- Health ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHealth = 100.0f;

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

	// --- Leveling ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 CurrentLevel = 1;

	// Multiply = 1.0 + (Level - 1) * DamageMultiplierPerLevel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float DamageMultiplierPerLevel = 0.1f;

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	// --- Functions ---
	UFUNCTION(BlueprintCallable, Category = "Status")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void Heal(float HealAmount);

	// calculated as 1.0 + (CurrentLevel - 1) * DamageMultiplierPerLevel
	UFUNCTION(BlueprintPure, Category = "Status")
	float GetDamageMultiplier() const;
};
