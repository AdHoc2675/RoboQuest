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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The current target actor
	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	AActor* CurrentTarget;

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
};
