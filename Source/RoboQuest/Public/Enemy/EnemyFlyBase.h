// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyFlyBase.generated.h"

/**
 * AEnemyFlyBase is Flying enemy base class
 */
UCLASS()
class ROBOQUEST_API AEnemyFlyBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyFlyBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// --- Config ---
	// Range within which the enemy detects the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectRange = 2000.0f; // Slightly larger range for flying enemies

	// Speed of rotation towards the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RotationSpeed = 5.0f;

	// --- Avoidance Settings ---
	// Minimum height from the ground to maintain
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float MinFlightHeight = 150.0f;

	// Distance to check for obstacles ahead
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float ObstacleCheckRange = 100.0f;

	// Strength of the avoidance force
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float AvoidanceForceMultiplier = 0.5f;

	// --- Hover Movement Settings ---
	// If true, the enemy will automatically move around the target
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	bool bEnableHovering = true;

	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float HoverMoveScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float HoverChangeInterval = 3.0f;

	// Distance to try to maintain from the target
	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float PreferredMinRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float PreferredMaxRange = 1200.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when actor is being removed or destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// The current target actor
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* CurrentTarget;

	// Current direction for hovering
	FVector CurrentHoverDirection;

	FTimerHandle HoverTimerHandle;

	// --- Functions ---
	// Logic to find a target (Player)
	virtual void FindTarget();

	// Logic to rotate towards the current target
	virtual void RotateTowardsTarget(float DeltaTime);

	// Checks if the target is valid and exists
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool HasValidTarget() const;

	// Checks if there are any obstacles between this enemy and the target
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanSeeTarget() const;

    // --- New Function ---
    // Calculates a vector to steer away from obstacles (Ground & Walls)
    UFUNCTION(BlueprintCallable, Category = "AI|Movement")
    FVector CalculateObstacleAvoidance();

	// Calculates a new random direction for intelligent hovering
	virtual void PickNewHoverDirection();
};
