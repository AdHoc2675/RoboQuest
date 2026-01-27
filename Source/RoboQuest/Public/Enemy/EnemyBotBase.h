// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyBotBase.generated.h"

/**
 * AEnemyBotBase: Grounded enemy using tracks/wheels.
 * Characteristics:
 * - Does NOT strafe (no side-stepping).
 * - Rotates whole body to face target (or movement direction).
 * - Moves linearly towards/away from target.
 */
UCLASS()
class ROBOQUEST_API AEnemyBotBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyBotBase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// --- Config ---
	
	// Distance to detect player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectRange = 1500.0f;

	// Distance to start attacking (and stop moving closer if preferred)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 800.0f;

	// Speed of body rotation (degrees per second)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RotationSpeed = 50.0f; // Bots might turn slower/heavier than pawns

	// If closer than this, the bot might back up (if implemented) or just stop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StopDistance = 400.0f;

	// --- State ---
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* CurrentTarget;

	// --- Functions ---
public:
	// Check line of sight
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanSeeTarget() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget() const { return CurrentTarget; }

protected:
	// Find nearest player
	virtual void FindTarget();

	// Tank-like rotation: Rotate Actor to face Target
	virtual void RotateTowardsTarget(float DeltaTime);

	// Tank-like movement: Move Forward/Backward only
	virtual void MoveTowardsTarget(float DeltaTime);

	// Check if we have a live target
	bool HasValidTarget() const;
};