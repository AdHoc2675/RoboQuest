// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/RoboQuestAbility.h"
#include "Ability_HomingMissile.generated.h"

class ARoboQuestHomingProjectile;

/**
 * Quickly searches for nearby enemies and fires a homing missile at the nearest target.
 */
UCLASS()
class ROBOQUEST_API UAbility_HomingMissile : public URoboQuestAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivate_Implementation() override;

protected:
	/* projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	TSubclassOf<ARoboQuestHomingProjectile> ProjectileClass;

	/* The radius within which to search for enemies */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float SearchRadius = 500.0f;

	/* Spawn offset from the character's location */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	FVector SpawnOffset = FVector(50.f, 0.f, 50.f);

private:
	/* Finds the nearest enemy component. */
	USceneComponent* FindNearestEnemy();
};
