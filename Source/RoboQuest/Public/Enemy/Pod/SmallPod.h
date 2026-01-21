// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyPodBase.h"
#include "SmallPod.generated.h"

/**
 * ASmallPod: A concrete implementation of a stationary pod enemy.
 * Inherits rotation logic from AEnemyPodBase and adds shooting mechanics (Projectile).
 */
UCLASS()
class ROBOQUEST_API ASmallPod : public AEnemyPodBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASmallPod();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends or actor is destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// --- Combat Stats ---
	
	// Firing Loop Rate (Seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 3.0f;

	// Base Damage for the projectile (Can be scaled by level later)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	// Projectile Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARoboQuestProjectile> ProjectileClass;

	// Socket name to spawn projectile from
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName MuzzleSocketName = TEXT("Canon_02");

protected:
	// Timer handle for automatic fire loop
	FTimerHandle FireTimerHandle;

	// Check conditions and attempt to fire
	void TryFire();

	// Spawn the actual projectile
	void FireProjectile();
};