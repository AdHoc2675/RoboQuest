// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyPawnBase.h"
#include "GunPawn.generated.h"

class ARoboQuestProjectile;

/**
 * AGunPawn: A grounded enemy that shoots projectiles at the player.
 * Inherits movement/ai from AEnemyPawnBase and adds shooting mechanics.
 */
UCLASS()
class ROBOQUEST_API AGunPawn : public AEnemyPawnBase
{
	GENERATED_BODY()

public:
	AGunPawn();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Override TakeDamage to handle Stagger logic
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// --- Combat Stats ---

	// Firing Loop Rate (Seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 2.0f;

	// Base Damage for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	// Projectile Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARoboQuestProjectile> ProjectileClass;

	// Socket name to spawn projectile from
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName MuzzleSocketName = TEXT("Muzzle_01");

	// --- Animations (Montages) ---

	// Animation played before shooting (e.g., aiming)
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
	float HitDamageThreshold = 20.0f;

protected:
	// Timer handle for automatic fire loop
	FTimerHandle FireLoopTimerHandle;

	// Timer handle for the delay between PreShoot and Shoot
	FTimerHandle AttackSequenceTimerHandle;

	// Track if we are currently in an attack sequence
	bool bIsAttacking = false;

	// --- Combat Functions ---

	// Check conditions and start the attack sequence
	void TryFire();

	// Called after PreShoot finishes, play Shoot animation and fire projectile
	void PerformShoot();

	// Spawn the actual projectile
	void FireProjectile();

	// Interrupt attack and play stagger
	void PlayHit();
};
