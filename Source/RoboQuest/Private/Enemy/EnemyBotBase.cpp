// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyBotBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyBotBase::AEnemyBotBase()
{
	// Bot setup: usually heavier, maybe slower acceleration
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.0f; // Slower than pawns
		GetCharacterMovement()->bUseControllerDesiredRotation = false; // We handle rotation manually
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void AEnemyBotBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBotBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	FindTarget();

	if (HasValidTarget())
	{
		// 1. Always rotate to face target (Bot behavior: Hull rotates to target)
		// NOTE: If you separate Hull and Turret later, this would rotate the whole actor (Hull).
		RotateTowardsTarget(DeltaTime);

		// 2. Move based on range (No Strafing)
		MoveTowardsTarget(DeltaTime);
	}
}

void AEnemyBotBase::FindTarget()
{
	// Simple Logic: Get Player 0
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (Player)
	{
		float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		if (Dist <= DetectRange)
		{
			CurrentTarget = Player;
		}
		else
		{
			// Lost target if too far (Optional persistence logic can go here)
			CurrentTarget = nullptr;
		}
	}
}

bool AEnemyBotBase::HasValidTarget() const
{
	return CurrentTarget && !CurrentTarget->IsHidden() && CurrentTarget->GetDistanceTo(this) <= DetectRange;
}

bool AEnemyBotBase::CanSeeTarget() const
{
	if (!CurrentTarget) return false;

	FHitResult Hit;
	FVector Start = GetActorLocation();
	FVector End = CurrentTarget->GetActorLocation();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Simple visibility trace
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	return !Hit.IsValidBlockingHit() || Hit.GetActor() == CurrentTarget;
}

void AEnemyBotBase::RotateTowardsTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	FVector Direction = CurrentTarget->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f; // Keep it planar

	if (Direction.IsNearlyZero()) return;

	FRotator TargetRot = Direction.Rotation();
	FRotator CurrentRot = GetActorRotation();

	// Smoothly interpolate rotation (simulates tank turn speed)
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);
	
	SetActorRotation(NewRot);
}

void AEnemyBotBase::MoveTowardsTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	float Dist = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

	// Simple State Machine for distance
	if (Dist > AttackRange)
	{
		// Forward: Chase
		// Only move if we are roughly facing the target (Tank logic)
		float AngleDot = FVector::DotProduct(GetActorForwardVector(), (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal());
		
		if (AngleDot > 0.7f) // +/- 45 degrees cone
		{
			AddMovementInput(GetActorForwardVector(), 1.0f);
		}
	}
	else if (Dist < StopDistance)
	{
		// Backward: Too close (Back up linearly)
		AddMovementInput(GetActorForwardVector(), -1.0f);
	}
	// Else: Hold position (Inside optimal range)
}