// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Pod/SmallPod.h"
#include "../../../RoboQuestProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"

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

	FVector SpawnLoc = GetActorLocation();
	
	// Use the Actor's rotation (guaranteed to face the target by EnemyPodBase)
	FRotator SpawnRot = GetActorRotation(); 

	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName) + GetActorForwardVector() * 30.0f;
	}
	else
	{
		SpawnLoc += GetActorForwardVector() * 30.0f;
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