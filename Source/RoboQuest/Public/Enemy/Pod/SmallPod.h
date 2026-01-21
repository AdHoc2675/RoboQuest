// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyPodBase.h"
#include "SmallPod.generated.h"

/**
 * ASmallPod: A concrete implementation of a stationary pod enemy.
 * Inherits rotation logic from AEnemyPodBase and adds shooting mechanics (Projectile) with animation sequence.
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

	// Base Damage for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	// Projectile Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARoboQuestProjectile> ProjectileClass;

	// Socket name to spawn projectile from
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName MuzzleSocketName = TEXT("Canon_02");

	// --- Animations (Montages) ---
	
	// Animation played before shooting (e.g., charging or aiming)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PreShootMontage;

	// Animation played when shooting (FireProjectile is called at the start of this)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ShootMontage;

	// Animation played when stunned/staggered
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;

	// Damage threshold to trigger stagger
	UPROPERTY(EditAnywhere, Category = "Combat")
	float HitDamageThreshold = 15.0f;

public:
	// Override TakeDamage to handle Stagger logic
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Timer handle for automatic fire loop (periodical)
	FTimerHandle FireLoopTimerHandle;

	// Timer handle for the delay between PreShoot and Shoot
	FTimerHandle AttackSequenceTimerHandle;

	// Track if we are currently in an attack sequence to avoid overlapping
	bool bIsAttacking = false;

	// 1. Check conditions and start the attack sequence
	void TryFire();

	// 2. Called after PreShoot finishes, plays Shoot animation and fires projectile
	void PerformShoot();

	// Spawn the actual projectile
	void FireProjectile();

	// Interrupt attack and play stagger
	void PlayHit();
};