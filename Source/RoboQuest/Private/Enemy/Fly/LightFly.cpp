// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Fly/LightFly.h"
#include "../../../RoboQuestProjectile.h"
#include "Components/StatusComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"

// Sets default values
ALightFly::ALightFly()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set default rotation speed defined in AEnemyFlyBase
	RotationSpeed = 8.0f; 
}

// Called when the game starts or when spawned
void ALightFly::BeginPlay()
{
	Super::BeginPlay();

	// Initialize status
	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats(TEXT("LightFly"), 1);
	}

	// Start Combat Loop
	GetWorld()->GetTimerManager().SetTimer(FireLoopTimerHandle, this, &ALightFly::TryFire, FireRate, true);

	// Start Hover Movement Loop
	PickNewHoverDirection(); // Pick initial direction
	GetWorld()->GetTimerManager().SetTimer(HoverTimerHandle, this, &ALightFly::PickNewHoverDirection, HoverChangeInterval, true);
}

void ALightFly::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(FireLoopTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
}

// Called every frame
void ALightFly::Tick(float DeltaTime)
{
	// Super::Tick handles FindTarget() and RotateTowardsTarget()
	Super::Tick(DeltaTime);
	
	if (!IsAlive()) return;

	// Add Movement Logic:
	// Only move if we have a valid target (active combat state)
	// CharacterMovement will handle the actual displacement based on input
	if (HasValidTarget())
	{
		AddMovementInput(CurrentHoverDirection, HoverMoveScale);
	}
}

void ALightFly::PickNewHoverDirection()
{
	if (!IsAlive()) return;

	// Start with a base random direction
	FVector NewDir = FMath::VRand();
	NewDir.Z *= 0.25f; // Flatten vertical movement slightly

	// Apply intelligent steering based on target distance
	if (HasValidTarget())
	{
		const float PreferredMinRange = 500.0f;
		const float PreferredMaxRange = 1200.0f;

		FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
		float Dist = ToTarget.Size();
		FVector DirToTarget = ToTarget.GetSafeNormal();

		if (Dist > PreferredMaxRange)
		{
			// Too far: Bias movement heavily towards the target
			// (Mix Random + Target Direction)
			NewDir = (NewDir * 0.5f + DirToTarget).GetSafeNormal();
		}
		else if (Dist < PreferredMinRange)
		{
			// Too close: Bias movement away from the target
			NewDir = (NewDir * 0.5f - DirToTarget).GetSafeNormal();
		}
		else
		{
			// In "Comfort Zone": Encourage Strafing (Orbiting)
			// Cross Product with UpVector gives a tangent (sideways) vector
			FVector OrbitDir = FVector::CrossProduct(DirToTarget, FVector::UpVector);

			// Randomly decide to orbit Left or Right
			if (FMath::RandBool())
			{
				OrbitDir *= -1.0f;
			}

			// Mix orbit direction with some randomness
			NewDir = (OrbitDir + NewDir * 0.5f).GetSafeNormal();
		}
	}

	CurrentHoverDirection = NewDir;
}

float ALightFly::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage >= HitDamageThreshold && IsAlive())
	{
		PlayHit();
	}
	
	return ActualDamage;
}

void ALightFly::PlayHit()
{
	if (HitMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
		bIsAttacking = false;
		
		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}
}

void ALightFly::TryFire()
{
	// Use parent helper functions: IsAlive(), HasValidTarget(), CanSeeTarget()
	if (!IsAlive() || !HasValidTarget() || !CanSeeTarget() || bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	float PreShootDuration = 0.0f;
	if (PreShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		PreShootDuration = GetMesh()->GetAnimInstance()->Montage_Play(PreShootMontage);
	}

	if (PreShootDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackSequenceTimerHandle, this, &ALightFly::PerformShoot, PreShootDuration, false);
	}
	else
	{
		PerformShoot();
	}
}

void ALightFly::PerformShoot()
{
	if (!IsAlive()) return;

	if (ShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage);
	}

	FireProjectile();

	// Reset state immediately or invoke via timer/animation notify if preferred
	bIsAttacking = false;
}

void ALightFly::FireProjectile()
{
	if (!ProjectileClass) return;

	FVector SpawnLoc = GetActorLocation();
	
	// Fly enemies might invoke fire from a specific forward direction
	FRotator SpawnRot = GetActorRotation();

	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName) + GetActorForwardVector() * 20.0f; // Small offset
	}
	else
	{
		SpawnLoc += GetActorForwardVector() * 50.0f;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = this;
	ActorSpawnParams.Instigator = GetInstigator();
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Use DetectRange from parent class
	ARoboQuestProjectile* Projectile = GetWorld()->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, SpawnRot, ActorSpawnParams);

	if (Projectile)
	{
		Projectile->InitializeProjectile(AttackDamage, DetectRange, 1.0f);
	}
}

