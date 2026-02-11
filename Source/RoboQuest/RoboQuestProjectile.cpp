// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoboQuestProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBase.h" // Include EnemyBase to check for friendly fire
#include "RoboQuestCharacter.h"

ARoboQuestProjectile::ARoboQuestProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ARoboQuestProjectile::OnHit); // set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

    // Ignore collision with other Projectiles to prevent mid-air blocking
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ARoboQuestProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		if (OtherActor->IsA(ARoboQuestProjectile::StaticClass()))
		{
			return;
		}

		// Friendly Fire Prevention: Check if both the Shooter and the Victim are Enemies
		AActor* ProjectileOwner = GetOwner();
		if (ProjectileOwner)
		{
			// Try to cast both actors to AEnemyBase (or check class type)
			bool bIsOwnerEnemy = ProjectileOwner->IsA(AEnemyBase::StaticClass());
			bool bIsHitEnemy = OtherActor->IsA(AEnemyBase::StaticClass());

			// If both are enemies, simply destroy the projectile without applying damage
			if (bIsOwnerEnemy && bIsHitEnemy)
			{
				Destroy();
				return;
			}
		}

		if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
		{
			if (Enemy->bIsBoss)
			{
				// Check Tags
				if (Tags.Contains(FName("Ability_Shotgun")) || Tags.Contains(FName("Ability_Missile")))
				{
					if (ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(GetOwner()))
					{
						if (Player->OnBossHitByAbility.IsBound())
						{
							Player->OnBossHitByAbility.Broadcast();
						}
					}
				}
			}
		}

		TSubclassOf<UDamageType> DmgType = ProjectileDamageType;
		if (!DmgType)
		{
			DmgType = UDamageType::StaticClass();
		}

		// Do not directly modify the variables of the other actor (e.g., HP). Use the engine's standard functions instead.
		UGameplayStatics::ApplyDamage(
			OtherActor,                     // The actor being hit
			Damage,                         // Amount of damage
			GetInstigatorController(),      // Controller of the instigator (used for kill logs, etc.)
			this,                           // The damage causer (the projectile itself)
			DmgType      // Damage type (change to fire, explosion, etc. if needed)
		);

		Destroy();
	}
}

void ARoboQuestProjectile::InitializeProjectile(float NewDamage, float NewRange, float NewCritMul, TSubclassOf<UDamageType> InDamageType)
{
	Damage = NewDamage;
	RangeMeter = NewRange;
	CritDamageMultiplier = NewCritMul;
	ProjectileDamageType = InDamageType;

	if (!ProjectileDamageType)
	{
		ProjectileDamageType = UDamageType::StaticClass();
	}

	// Update velocity based on InitialSpeed when initialized
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}

	// Ignore collision with the owner who fired this projectile to prevent self-damage/instant stop
	if (GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void ARoboQuestProjectile::SetExtraProperties(bool bInCanPierce, int32 InMaxBounces)
{
	bCanPierce = bInCanPierce;
	MaxBounces = InMaxBounces;

	if (ProjectileMovement)
	{
		ProjectileMovement->bShouldBounce = (MaxBounces > 0);
		// Bounciness setting (e.g. 0.6)
		if (MaxBounces > 0)
		{
			ProjectileMovement->Bounciness = 1.0f;
			ProjectileMovement->Friction = 0.0f;
		}
	}
}