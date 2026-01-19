// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoboQuestProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ARoboQuestProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	ARoboQuestProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	// Projectile properties
	void InitializeProjectile(float NewDamage, float NewRange, float NewCritMul);

	// Damage dealt by this projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Damage;
	// Effective range in meters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float RangeMeter;
	// Critical damage multiplier
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float CritDamageMultiplier;
};

