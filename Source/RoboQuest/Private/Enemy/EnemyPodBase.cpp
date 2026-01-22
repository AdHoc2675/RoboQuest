// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyPodBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AEnemyPodBase::AEnemyPodBase()
{
	// Disable movement since it's a stationary unit
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	// Prevent the character from adhering to the controller's rotation yaw (We handle rotation manually)
	bUseControllerRotationYaw = false;
}

void AEnemyPodBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Stop logic if dead
	if (!IsAlive()) return;

	FindTarget();

	// Rotate only if we have a valid target and can actually see it (not blocked by walls)
	if (HasValidTarget() && CanSeeTarget())
	{
		RotateTowardsTarget(DeltaTime);
	}
}

void AEnemyPodBase::FindTarget()
{
	// Simple targeting: Get Player 0
	// (Can be improved with AI Perception later)
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

void AEnemyPodBase::RotateTowardsTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	// Calculate the rotation needed to look at the target
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTarget->GetActorLocation());
    
    // Smoothly interpolate current rotation to target rotation
	FRotator NewRot = FMath::RInterpTo(GetActorRotation(), LookAtRot, DeltaTime, RotationSpeed);
	SetActorRotation(NewRot);
}

bool AEnemyPodBase::HasValidTarget() const
{
	return (CurrentTarget != nullptr);
}

bool AEnemyPodBase::CanSeeTarget() const
{
	if (!CurrentTarget) return false;

	FHitResult Hit;
	
    // Start position: Eye location or Actor location with offset
    // (Using Mesh socket or EyePosition is better for accuracy, currently using ActorLocation + height offset)
	FVector Start = GetActorLocation() + FVector(0, 0, 50.0f); 
	FVector End = CurrentTarget->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);          // Ignore self
	Params.AddIgnoredActor(CurrentTarget); // Ignore the target itself (to check against obstacles only)

	// Check using Visibility channel (Walls usually block Visibility)
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// Debug line
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false);

	// If bHit is true, it means there is an obstacle (Wall) -> Not visible
	// If bHit is false, the trace went through -> Visible
	return !bHit;
}
