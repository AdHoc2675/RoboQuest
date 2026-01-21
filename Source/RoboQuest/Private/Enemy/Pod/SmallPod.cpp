// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Pod/SmallPod.h"
#include "../../../RoboQuestProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

ASmallPod::ASmallPod()
{
}

void ASmallPod::BeginPlay()
{
	Super::BeginPlay();

	// Initialize stats from DataTable (RowName: "SmallPod", Level: 1)
	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats(TEXT("SmallPod"), 1);
	}

	// Start the firing loop
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ASmallPod::TryFire, FireRate, true);
}

// [Added] Clean up resources when the actor is removed/destroyed
void ASmallPod::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason); // Don't forget to call Super

	// Ensure the timer is cleared to prevent any function calls on a destroying actor
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void ASmallPod::TryFire()
{
	// If dead, stop firing
	if (!IsAlive())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		return;
	}

	// Only fire if we have a valid target AND we can see it (Line of Sight)
	// These functions are inherited from AEnemyPodBase
	if (HasValidTarget() && CanSeeTarget())
	{
		FireProjectile();
	}
}

void ASmallPod::FireProjectile()
{
	if (!ProjectileClass) return;

	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = GetActorRotation(); 

	// Use Muzzle socket if it exists on the mesh
	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName) + GetActorForwardVector() * 30.0f;
		// SpawnRot = GetMesh()->GetSocketRotation(MuzzleSocketName) + FRotator(0.0f, -90.0f, 0.0f);
	}
	else
	{
		// Fallback: Spawn slightly forward to avoid self-collision
		SpawnLoc += GetActorForwardVector() * 30.0f;
	}

	// Spawn parameters
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;
	ActorSpawnParams.Instigator = GetInstigator();
	
	// Adjust collision handling to always spawn even if colliding with something
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn the projectile
	ARoboQuestProjectile* Projectile = GetWorld()->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, SpawnRot, ActorSpawnParams);
	
	if (Projectile)
	{
		// Initialize projectile stats
		// (Damage, Range, CritMultiplier)
		// We use DetectRange from base class as the projectile range
		Projectile->InitializeProjectile(AttackDamage, DetectRange, 1.0f);
	}
}