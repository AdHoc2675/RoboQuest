// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusComponent.h"
#include "EnemyBase.generated.h"

class AHealingCell;

UCLASS()
class ROBOQUEST_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStatusComponent* StatusComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyHealthComponent* Health;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyTargetComponent* Target;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyDropComponent* Drop;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyWeaponComponent* Weapon;

	//// --- Data ---
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	//UEnemyArchetypeDataAsset* Archetype;

	bool bIsDead = false;

	// --- Drops ---
	
	// Class of the Healing Cell to drop
	UPROPERTY(EditDefaultsOnly, Category = "Drops")
	TSubclassOf<AActor> HealingCellClass; // Use TSubclassOf<AActor> or include HealingCell.h

	// Number of cells to drop on death
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Drops")
	int32 DropCount = 3;

public:	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const { return !bIsDead; }

protected:
	// bind to health changed event
	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float ScratchHealth, float MaxHealth);

	// Spawns healing cells
	virtual void SpawnDrops();
};
