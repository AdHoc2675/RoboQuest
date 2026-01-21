// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoboQuestProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StatusComponent.h"
#include "Kismet/GameplayStatics.h"

ARoboQuestProjectile::ARoboQuestProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ARoboQuestProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

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
		// Do not directly modify the variables of the other actor (e.g., HP). Use the engine's standard functions instead.
		UGameplayStatics::ApplyDamage(
			OtherActor,                     // The actor being hit
			Damage,                         // Amount of damage
			GetInstigatorController(),      // Controller of the instigator (used for kill logs, etc.)
			this,                           // The damage causer (the projectile itself)
			UDamageType::StaticClass()      // Damage type (change to fire, explosion, etc. if needed)
		);

		Destroy();
	}
}

void ARoboQuestProjectile::InitializeProjectile(float NewDamage, float NewRange, float NewCritMul)
{
	Damage = NewDamage;
	RangeMeter = NewRange;
	CritDamageMultiplier = NewCritMul;

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}

}