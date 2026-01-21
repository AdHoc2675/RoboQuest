// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy/EnemyFlyBase.h"
#include "LightFly.generated.h"

class ARoboQuestProjectile;

/**
 * ALightFly: Flying enemy that focuses on combat mechanics.
 * Movement and hovering logic are handled by AEnemyFlyBase.
 */
UCLASS()
class ROBOQUEST_API ALightFly : public AEnemyFlyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALightFly();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called when the game ends or actor is destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Override TakeDamage to handle Stagger logic
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// --- Combat Stats ---

	// Firing Loop Rate (Seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 2.0f; // Typical fly fire rate

	// Base Damage for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 5.0f;

	// Projectile Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ARoboQuestProjectile> ProjectileClass;

	// Socket name to spawn projectile from
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName MuzzleSocketName = TEXT("Head"); // Adjust based on mesh

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
	float HitDamageThreshold = 10.0f;

protected:
	// Timer for attack loop
	FTimerHandle FireLoopTimerHandle;
	
	// Timer for sequence: PreShoot -> Shoot
	FTimerHandle AttackSequenceTimerHandle;

	// Track if we are currently in an attack sequence to avoid overlapping
	bool bIsAttacking = false;

	// --- Combat Functions ---

	// 1. Check conditions and start the attack sequence
	void TryFire();

	// 2. Called after PreShoot finishes, plays Shoot animation and fires projectile
	void PerformShoot();

	// Spawn the actual projectile
	void FireProjectile();

	// Interrupt attack and play stagger
	void PlayHit();

	// Note: Hovering logic is inherited from AEnemyFlyBase (bEnableHovering, HoverMoveScale, etc.)
};
