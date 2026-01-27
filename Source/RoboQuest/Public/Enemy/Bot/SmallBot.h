// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBotBase.h"
#include "SmallBot.generated.h"

class ARoboQuestProjectile;

/**
 * ASmallBot: A basic grounded enemy unit.
 * Inherits tank-like movement from AEnemyBotBase.
 * Implements projectile firing with Pre-Shoot anticipation (Telegraphing).
 */
UCLASS()
class ROBOQUEST_API ASmallBot : public AEnemyBotBase
{
	GENERATED_BODY()

public:
	ASmallBot();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// --- Combat Stats ---

	// Time between attack attempts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 4.0f;

	// Damage dealt by projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.0f;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ARoboQuestProjectile> ProjectileClass;

	// Socket name for muzzle location
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName MuzzleSocketName = TEXT("R_Canon_Scale02");

	// --- Animation ---

	// Animation played before shooting (Telegraphing/Charging)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PreShootMontage;

	// Animation played when shooting (FireProjectile is called at the start of this)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ShootMontage;

	// Animation played when damaged (Stagger)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HitMontage;
	
	// Damage threshold to trigger stagger reaction
	UPROPERTY(EditAnywhere, Category = "Combat")
	float HitDamageThreshold = 10.0f;

	// Override to handle hit reactions
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	// Timer for the periodical firing loop
	FTimerHandle FireLoopTimerHandle;

	// Timer for the delay between PreShoot and Shoot
	FTimerHandle AttackSequenceTimerHandle;

	// Prevent spamming attacks or overlapping sequences
	bool bIsAttacking = false;

	// 1. Check conditions and start the attack sequence (Plays PreShoot)
	void TryFire();

	// 2. Called after PreShoot finishes. Plays Shoot anim and fires projectile.
	void PerformShoot();

	// Spawn the actual projectile
	void FireProjectile();

	// Interrupt attack and play stagger
	void PlayHit();
};
