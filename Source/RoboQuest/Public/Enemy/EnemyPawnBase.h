// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyPawnBase.generated.h"

/**
 * AEnemyPawnBase is Grounded enemy base class
 * Handles ground movement logic, target detection, orientation and basic combat movement (range maintain + strafing).
 */
UCLASS()
class ROBOQUEST_API AEnemyPawnBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyPawnBase();

	virtual void Tick(float DeltaTime) override;

	// --- Config ---
	// Range within which the enemy detects the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectRange = 1500.0f;

	// Distance to stop moving towards the target (Attack Range)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 500.0f;

	// Rotation speed (interpolation)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RotationSpeed = 10.0f;

	// --- Combat Movement Settings ---
	// If distance is greater than this, move closer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float PreferredMaxRange = 800.0f;

	// If distance is less than this, back away.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float PreferredMinRange = 400.0f;

	// How fast to strafe sideways (scale for AddMovementInput)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float StrafeSpeed = 1.0f;

	// How often to change strafing direction (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float StrafeChangeInterval = 2.0f;

	// Getter for current target (useful for AIController)
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTarget() const { return CurrentTarget; }

	// Checks visibility
	bool CanSeeTarget() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// The current target actor
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* CurrentTarget;

	// --- Functions ---
	// Logic to find a target (Player)
	virtual void FindTarget();

	// Logic to rotate towards the current target (Yaw only)
	virtual void RotateTowardsTarget(float DeltaTime);

	// Basic movement while in combat: maintain range and strafe
	virtual void CombatMove(float DeltaTime);

	// Basic direct movement towards target (fallback)
	virtual void MoveToTarget();

	// Pick a new strafing direction periodically
	void PickNewStrafeDirection();

	// Checks if target is valid
	bool HasValidTarget() const;

protected:
	// Timer handle for changing strafe direction
	FTimerHandle StrafeTimerHandle;

	// Current strafing direction multiplier (-1 left, 0 none, 1 right)
	float StrafeDirectionScale = 0.0f;
};
