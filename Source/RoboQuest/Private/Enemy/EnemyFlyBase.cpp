// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFlyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AEnemyFlyBase::AEnemyFlyBase()
{
	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;

	// Configure Character Movement for flying
	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 1000.f; // Deceleration when stopping
	GetCharacterMovement()->MaxFlySpeed = 600.f; // Adjust speed as needed

	// Configure Navigation settings for flying (if using NavMesh with flying support or similar)
	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
	GetCharacterMovement()->NavAgentProps.bCanJump = false;
	GetCharacterMovement()->NavAgentProps.bCanWalk = false;
	GetCharacterMovement()->NavAgentProps.bCanFly = true;

	// Rotate character to face movement direction
	bUseControllerRotationYaw = false; // Don't snap to controller by default
	GetCharacterMovement()->bUseControllerDesiredRotation = true; // Smoothly rotate to controller's desired rotation
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // Fast rotation
}

void AEnemyFlyBase::BeginPlay()
{
	Super::BeginPlay();

	// Ensure movement mode is set to Flying on start
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void AEnemyFlyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Stop logic if dead
	if (!IsAlive()) return;

	FindTarget();

	// Logic: If we have a target and line of sight, face them.
	// Note: In a full AI implementation, the AIController would handle movement and focus.
	// This manual rotation ensures the enemy faces the player even if stationary or strafing.
	if (HasValidTarget() && CanSeeTarget())
	{
		RotateTowardsTarget(DeltaTime);
	}
}

void AEnemyFlyBase::FindTarget()
{
	// Simple targeting: Get Player 0
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
			CurrentTarget = nullptr;
		}
	}
}

void AEnemyFlyBase::RotateTowardsTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	// Calculate the rotation needed to look at the target
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTarget->GetActorLocation());

	// If using Controller Desired Rotation, we might want to update ControlRotation instead
	// But for direct override referencing PodBase, direct ActorRotation works visually.
	// For smoother movement integration, consider creating an AIController that calls SetFocus(CurrentTarget).
	
	FRotator NewRot = FMath::RInterpTo(GetActorRotation(), LookAtRot, DeltaTime, RotationSpeed);
	
	// Lock Pitch/Roll if you want the enemy to stay upright (optional for drones)
	// NewRot.Pitch = 0.0f;
	// NewRot.Roll = 0.0f;

	SetActorRotation(NewRot);
}

bool AEnemyFlyBase::HasValidTarget() const
{
	return (CurrentTarget != nullptr);
}

bool AEnemyFlyBase::CanSeeTarget() const
{
	if (!CurrentTarget) return false;

	FHitResult Hit;

	// Start position: Slightly above actor location to avoid self-collision at feet
	FVector Start = GetActorLocation() + FVector(0, 0, 50.0f);
	FVector End = CurrentTarget->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);          // Ignore self
	Params.AddIgnoredActor(CurrentTarget); // Ignore the target itself

	// Check using Visibility channel
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// Debug line (Visible in Editor)
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 0.1f);

	return !bHit; // Returns true if no obstacles were hit
}

