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
	Super::BeginPlay(); // Base class starts hovering automatically!

	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats(TEXT("LightFly"), 1);
	}

	// Only manage Combat Loop here
	GetWorld()->GetTimerManager().SetTimer(FireLoopTimerHandle, this, &ALightFly::TryFire, FireRate, true);
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
	Super::Tick(DeltaTime); // Base class handles Movement/Rotation
	
	// No extra code needed here unless LightFly has special tick logic
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

