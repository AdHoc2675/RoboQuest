// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RoboQuestProjectile.h"
#include "RoboQuestHomingProjectile.generated.h"

/**
 * A homing projectile that follows a target.
 */
UCLASS()
class ROBOQUEST_API ARoboQuestHomingProjectile : public ARoboQuestProjectile
{
	GENERATED_BODY()
	
public:
	ARoboQuestHomingProjectile();

	/** Sets the homing target for the projectile. */
	void SetHomingTarget(USceneComponent* TargetComponent);

protected:
	/* Homing force magnitude applied to the projectile. */
	UPROPERTY(EditDefaultsOnly, Category = "Homing")
	float HomingMagnitude = 20000.0f;
};
