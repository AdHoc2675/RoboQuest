// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Pod/SmallPod.h"
#include "../../../RoboQuestProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"

ASmallPod::ASmallPod()
{
	RotationSpeed = 7.5f;
}

void ASmallPod::BeginPlay()
{
	Super::BeginPlay();

	// Initialize stats from DataTable (RowName: "SmallPod", Level: 1)
	if (StatusComponent2)
	{
		StatusComponent2->InitializeEnemyStats(TEXT("SmallPod"), 1);
	}

	// Start the firing loop (Calls TryFire periodically)
	GetWorld()->GetTimerManager().SetTimer(FireLoopTimerHandle, this, &ASmallPod::TryFire, FireRate, true);
}

void ASmallPod::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear all timers
	GetWorld()->GetTimerManager().ClearTimer(FireLoopTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
}

float ASmallPod::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// If damage exceeds threshold and we are not already dead, trigger stagger
	if (ActualDamage >= HitDamageThreshold && IsAlive())
	{
		PlayHit();
	}

	return ActualDamage;
}

void ASmallPod::PlayHit()
{
	if (HitMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// Interrupt any ongoing attack sequence
		GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
		bIsAttacking = false;

		// Play Stagger Montage
		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}
}

void ASmallPod::TryFire()
{
	// Conditions: Alive, Valid Target, Line of Sight, and Not already attacking
	if (!IsAlive() || !HasValidTarget() || !CanSeeTarget() || bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	// Play PreShoot Animation
	float PreShootDuration = 0.0f;
	if (PreShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		PreShootDuration = GetMesh()->GetAnimInstance()->Montage_Play(PreShootMontage);
	}

	// Set Timer to trigger PerformShoot after PreShoot finishes
	// If duration is 0 (no montage), fire immediately
	if (PreShootDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackSequenceTimerHandle, this, &ASmallPod::PerformShoot, PreShootDuration, false);
	}
	else
	{
		PerformShoot();
	}
}

void ASmallPod::PerformShoot()
{
	if (!IsAlive()) return;

	// Play Shoot Animation
	if (ShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage);
	}

	// Spawn Projectile (at the start of Shoot animation)
	FireProjectile();

	// Reset Attack State
	// (Optional: You could also wait for ShootMontage to finish before resetting bIsAttacking)
	bIsAttacking = false;
}

void ASmallPod::FireProjectile()
{
	if (!ProjectileClass) return;

	// 1. Determine Spawn Location (Muzzle)
	FVector SpawnLoc = GetActorLocation();
	
	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		// Use the socket location if it exists
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		// Fallback
		SpawnLoc += GetActorForwardVector() * 30.0f;
	}

	// 2. Determine Spawn Rotation (Aim at Target)
	FRotator SpawnRot = GetActorRotation(); // Default to facing direction

	if (HasValidTarget() && CurrentTarget)
	{
		// Calculate vector from Muzzle to Target
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		
		// Optional: Aim slightly at the center of the target (e.g., chest height)
		// TargetLoc.Z += 40.0f; 

		FVector DirectionToTarget = TargetLoc - SpawnLoc;
		
		// Create rotation from direction vector
		SpawnRot = DirectionToTarget.Rotation();
	}
    else
    {
        // If no target, just fire forward (based on actor rotation)
        SpawnRot = GetActorRotation();
    }

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;
	ActorSpawnParams.Instigator = GetInstigator();
	
	// Always spawn collision handling
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARoboQuestProjectile* Projectile = GetWorld()->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, SpawnRot, ActorSpawnParams);
	
	if (Projectile)
	{
		Projectile->InitializeProjectile(AttackDamage, DetectRange, 1.0f);
	}
}