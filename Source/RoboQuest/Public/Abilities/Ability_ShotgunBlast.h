// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/RoboQuestAbility.h"
#include "Ability_ShotgunBlast.generated.h"

class ARoboQuestProjectile;

/**
 * F Skill: Fires multiple projectiles (pellets) in a cone in front of the character.
 */
UCLASS()
class ROBOQUEST_API UAbility_ShotgunBlast : public URoboQuestAbility
{
	GENERATED_BODY()

public:
	virtual void OnActivate_Implementation() override;

protected:
	/* The class of the standard projectile to be fired */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	TSubclassOf<ARoboQuestProjectile> ProjectileClass;

	/* Number of projectiles (pellets) */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	int32 PelletCount = 8;

	/* Spread angle (degrees) */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float SpreadAngle = 10.0f;

	/* Projectile damage */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float DamagePerPellet = 10.0f;
};
