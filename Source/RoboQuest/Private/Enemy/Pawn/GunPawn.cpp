// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Pawn/GunPawn.h"
#include "../../../RoboQuestProjectile.h"
#include "Components/StatusComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"

AGunPawn::AGunPawn()
{
	// Adjust base stats for GunPawn
	DetectRange = 1500.0f;
	AttackRange = 800.0f;     // Stop closer than detect range to shoot
	RotationSpeed = 10.0f;
	
	// Combat Move Settings
	PreferredMinRange = 400.0f;
	PreferredMaxRange = 900.0f;
}

void AGunPawn::BeginPlay()
{
	Super::BeginPlay();

	// Initialize stats
	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats(TEXT("GunPawn"), 1);
	}

	// Start the firing loop (Calls TryFire periodically)
	GetWorld()->GetTimerManager().SetTimer(FireLoopTimerHandle, this, &AGunPawn::TryFire, FireRate, true);
}

void AGunPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(FireLoopTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
}

float AGunPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Stagger Logic
	if (ActualDamage >= HitDamageThreshold && IsAlive())
	{
		PlayHit();
	}

	return ActualDamage;
}

void AGunPawn::PlayHit()
{
	if (HitMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// Interrupt attack
		GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
		bIsAttacking = false;

		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}
}

void AGunPawn::TryFire()
{
	// Don't fire if dead, no target, can't see target, or already attacking
	if (!IsAlive() || !HasValidTarget() || !CanSeeTarget() || bIsAttacking)
	{
		return;
	}

	// Check distance - only fire if within effective range
	float Dist = FVector::Dist(GetActorLocation(), GetTarget()->GetActorLocation());
	if (Dist > DetectRange) return;

	bIsAttacking = true;

	// Play Pre-Shoot (Aim/Charge)
	float PreShootDuration = 0.0f;
	if (PreShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		PreShootDuration = GetMesh()->GetAnimInstance()->Montage_Play(PreShootMontage);
	}

	// Schedule Shoot
	if (PreShootDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackSequenceTimerHandle, this, &AGunPawn::PerformShoot, PreShootDuration, false);
	}
	else
	{
		PerformShoot();
	}
}

void AGunPawn::PerformShoot()
{
	if (!IsAlive()) return;

	// Play Shoot Animation
	if (ShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage);
	}

	// Fire Projectile
	FireProjectile();

	// Reset attack state
	bIsAttacking = false;
}

void AGunPawn::FireProjectile()
{
	if (!ProjectileClass) return;

	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = GetActorRotation();

	// Use Mesh Socket if available
	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName);
		
		// Optional: Adjust rotation to look exactly at target center (compensate for socket offset)
		if (GetTarget())
		{
			// Aim slightly up or at center of mass
			FVector TargetLoc = GetTarget()->GetActorLocation(); 
			SpawnRot = (TargetLoc - SpawnLoc).Rotation();
		}
	}
	else
	{
		// Fallback: spawn in front of actor
		SpawnLoc += GetActorForwardVector() * 50.0f + FVector(0,0,50.0f);
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;
	ActorSpawnParams.Instigator = GetInstigator();
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARoboQuestProjectile* Projectile = GetWorld()->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, SpawnRot, ActorSpawnParams);

	if (Projectile)
	{
		// Pass damage and range
		Projectile->InitializeProjectile(AttackDamage, DetectRange, 1.0f);
	}
}

