// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusComponent.h"
#include "EnemyBase.generated.h"

class AHealingCell;
class UDamageTextWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, AEnemyBase*, DeadEnemy);

UCLASS()
class ROBOQUEST_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStatusComponent* StatusComponent2;

	// Flag to determine if this enemy is a Boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
	bool bIsBoss = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsDead = false;

	// --- Audio ---

	// Sound played when this enemy fires a weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* ShootSound;

	// Sound played when taking damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* HitSound;

	// Sound played when dying
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* DeathSound;

	// Helper function to play shoot sound (called by subclasses)
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayShootSound();

	// --- Drops ---
	
	// Class of the Healing Cell to drop
	UPROPERTY(EditDefaultsOnly, Category = "Drops")
	TSubclassOf<AActor> HealingCellClass; // Use TSubclassOf<AActor> or include HealingCell.h

	// Number of cells to drop on death
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Drops")
	int32 DropCount = 3;

	// --- Power Drops ---

	// Class of the Power Cell to drop (For Upgrade Currency)
	UPROPERTY(EditDefaultsOnly, Category = "Drops")
	TSubclassOf<AActor> PowerCellClass;

	// Probability to drop Power Cells (0.0 to 1.0)
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Drops")
	float PowerDropChance = 0.1f;

	// Number of Power Cells to drop if chance succeeds
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Drops")
	int32 PowerDropCount = 1;

	// --- UI / Feedback ---
	
	// Widget Class to spawn for Floating Damage Numbers
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextWidget> DamageTextWidgetClass;

public:	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const { return !bIsDead; }

	// Enemy Died Event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyDied OnEnemyDied;

protected:
	// bind to health changed event
	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float ScratchHealth, float MaxHealth);

	// Spawns healing cells
	virtual void SpawnDrops();

	// Helper to spawn the widget
	void ShowFloatingDamage(float Damage, bool bCritical, FLinearColor TextColor = FLinearColor::White);

};
