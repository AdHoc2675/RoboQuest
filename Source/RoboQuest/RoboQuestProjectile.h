// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoboQuestProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UDamageType;
class UNiagaraComponent;

UCLASS(config=Game)
class ARoboQuestProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARoboQuestProjectile();

	/** Niagara Trail Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UNiagaraComponent* TrailFXComponent;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	// Projectile properties
	void InitializeProjectile(float NewDamage, float NewRange, float NewCritMul, TSubclassOf<UDamageType> InDamageType = nullptr);

	// The damage type this projectile inflicts
	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<UDamageType> ProjectileDamageType;

	// Damage dealt by this projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Damage;
	// Effective range in meters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float RangeMeter;
	// Critical damage multiplier
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float CritDamageMultiplier;
	// Max number of bounces allowed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	int32 MaxBounces = 0;
	// Can this projectile pierce enemies?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bCanPierce = false;
	// Modify stats dynamically
	UFUNCTION(BlueprintCallable)
	void SetExtraProperties(bool bInCanPierce, int32 InMaxBounces);
};

