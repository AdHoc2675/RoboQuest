// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Bot/SmallBot.h"
#include "RoboQuest/RoboQuestProjectile.h"
#include "Components/StatusComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"

ASmallBot::ASmallBot()
{
	// Configure default movement stats for "SmallBot"
	if (GetCharacterMovement())
	{
		// Bots are usually heavier/slower
		GetCharacterMovement()->MaxWalkSpeed = 350.0f; 
	}

	// Default AI settings (From EnemyBotBase)
	DetectRange = 1200.0f;
	AttackRange = 700.0f;  
	RotationSpeed = 80.0f; 
}

void ASmallBot::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Stats (Stats Row Name: "SmallBot")
	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats(TEXT("SmallBot"), 1);
	}

	// Start firing loop (with random initial delay to desync multiple bots)
	GetWorld()->GetTimerManager().SetTimer(FireLoopTimerHandle, this, &ASmallBot::TryFire, FireRate, true, FMath::RandRange(0.5f, 1.5f));
}

void ASmallBot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// Clear timers to prevent crashes
	GetWorld()->GetTimerManager().ClearTimer(FireLoopTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
}

float ASmallBot::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Stagger Logic
	if (ActualDamage >= HitDamageThreshold && IsAlive())
	{
		PlayHit();
	}

	return ActualDamage;
}

void ASmallBot::TryFire()
{
	// Conditions: Alive, Valid Target, Line of Sight, Not already busy
	if (!IsAlive() || !CanSeeTarget() || bIsAttacking)
	{
		return;
	}

	// Double check alignment: Don't shoot if we are facing completely away
	if (CurrentTarget)
	{
		FVector ToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		if (FVector::DotProduct(GetActorForwardVector(), ToTarget) < 0.7f) // +/- 45 degrees
		{
			return; // Still turning
		}
	}

	bIsAttacking = true;

	// 1. Play PreShoot (Warning/Telegraph)
	float PreShootDuration = 0.0f;
	if (PreShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		PreShootDuration = GetMesh()->GetAnimInstance()->Montage_Play(PreShootMontage);
	}

	// 2. Schedule PerformShoot
	if (PreShootDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackSequenceTimerHandle, this, &ASmallBot::PerformShoot, PreShootDuration, false);
	}
	else
	{
		PerformShoot();
	}
}

void ASmallBot::PerformShoot()
{
	if (!IsAlive()) return;

	// 3. Play Shoot Animation
	if (ShootMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ShootMontage);
	}

	// 4. Fire logic
	FireProjectile();

	// Reset state (Allow next attack loop)
	// You could also wait for ShootMontage length here if you want a longer cooldown
	bIsAttacking = false;
}

void ASmallBot::FireProjectile()
{
	if (!ProjectileClass) return;

	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = GetActorRotation();

	// Calculate Muzzle Location
	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(MuzzleSocketName);
		
		// Optional: Aim Correction to target center
		if (CurrentTarget)
		{
			// Aim at target location
			SpawnRot = (CurrentTarget->GetActorLocation() - SpawnLoc).Rotation();
		}
		else
		{
			// Use socket rotation
			SpawnRot = GetMesh()->GetSocketRotation(MuzzleSocketName);
		}
	}
	else
	{
		// Fallback setup
		SpawnLoc += GetActorForwardVector() * 50.0f + FVector(0,0,30);
		SpawnRot = GetActorRotation();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARoboQuestProjectile* Projectile = GetWorld()->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, SpawnRot, SpawnParams);
	if (Projectile)
	{
		Projectile->InitializeProjectile(AttackDamage, DetectRange, 1.0f);
	}
}

void ASmallBot::PlayHit()
{
	// Interrupt attack sequence on heavy hit
	GetWorld()->GetTimerManager().ClearTimer(AttackSequenceTimerHandle);
	bIsAttacking = false;

	if (HitMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	}
}

