// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyPodBase.generated.h"

/**
 * AEnemyPodBase: A base class for stationary enemies that don't move but rotate to face a target.
 * Specific attack logic is not defined here (should be defined in derived classes).
 */
UCLASS()
class ROBOQUEST_API AEnemyPodBase : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyPodBase();

public:
	virtual void Tick(float DeltaTime) override;

	// --- Config ---
	// Range within which the enemy detects the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectRange = 1500.0f;

	// Speed of rotation towards the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RotationSpeed = 5.0f;

protected:
	// The current target actor
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* CurrentTarget;

	// --- Functions ---
	// Logic to find a target
	virtual void FindTarget();

	// Logic to rotate towards the current target
	virtual void RotateTowardsTarget(float DeltaTime);

	// Checks if the target is valid and exists
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool HasValidTarget() const;

	// Checks if there are any obstacles between this enemy and the target
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanSeeTarget() const;
};
