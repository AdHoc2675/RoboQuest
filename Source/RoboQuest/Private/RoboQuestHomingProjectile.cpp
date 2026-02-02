// Fill out your copyright notice in the Description page of Project Settings.


#include "RoboQuestHomingProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ARoboQuestHomingProjectile::ARoboQuestHomingProjectile()
{

	if (ProjectileMovement)
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingAccelerationMagnitude = HomingMagnitude;
		// Disable gravity for homing projectile
		ProjectileMovement->ProjectileGravityScale = 0.0f; 
	}
}

void ARoboQuestHomingProjectile::SetHomingTarget(USceneComponent* TargetComponent)
{
	if (ProjectileMovement && TargetComponent)
	{
		ProjectileMovement->HomingTargetComponent = TargetComponent;
		ProjectileMovement->bIsHomingProjectile = true;
	}
}