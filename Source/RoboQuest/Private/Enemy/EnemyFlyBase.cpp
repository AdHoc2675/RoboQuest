// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFlyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

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

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	// Start Hovering Logic
	if (bEnableHovering)
	{
		PickNewHoverDirection();
		GetWorld()->GetTimerManager().SetTimer(HoverTimerHandle, this, &AEnemyFlyBase::PickNewHoverDirection, HoverChangeInterval, true);
	}
}

void AEnemyFlyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
}

void AEnemyFlyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	FindTarget();

	if (HasValidTarget())
	{
		// 1. Look at Target
		if (CanSeeTarget())
		{
			RotateTowardsTarget(DeltaTime);
		}

		// 2. Move (Hovering + Avoidance)
		if (bEnableHovering)
		{
			// Get Avoidance Vector (Push away from walls/ground)
			FVector Avoidance = CalculateObstacleAvoidance();
			
			// Combine Intent (Hover) + Safety (Avoidance)
			// Avoidance has higher priority, so we simply add it.
			FVector FinalDirection = (CurrentHoverDirection + Avoidance).GetSafeNormal();

			AddMovementInput(FinalDirection, HoverMoveScale);
		}
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

FVector AEnemyFlyBase::CalculateObstacleAvoidance()
{
	FVector AvoidanceVector = FVector::ZeroVector;
	FVector ActorLocation = GetActorLocation();
	FVector Velocity = GetVelocity();
    
    // Use ForwardVector if velocity is too small (e.g. starting move)
    FVector MoveDir = Velocity.IsNearlyZero() ? GetActorForwardVector() : Velocity.GetSafeNormal();

	// 1. Ground Avoidance (Maintain Height)
	FHitResult GroundHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

    // Check downwards
	FVector DownEnd = ActorLocation - FVector(0, 0, MinFlightHeight * 1.5f);
	bool bHitGround = GetWorld()->LineTraceSingleByChannel(GroundHit, ActorLocation, DownEnd, ECC_WorldStatic, Params);

	if (bHitGround)
	{
		// Force proportional to how close we are to the ground
        // If dist is 0 (touching ground), force is max. If dist is MinHeight, force is lower.
		float DistToGround = GroundHit.Distance;
        if (DistToGround < MinFlightHeight)
        {
            float PushRatio = 1.0f - (DistToGround / MinFlightHeight);
            AvoidanceVector += FVector::UpVector * PushRatio * AvoidanceForceMultiplier;
        }
	}

	// 2. Wall/Obstacle Avoidance
	FHitResult WallHit;
    // Sphere trace is better than line trace for volume checking
    float SphereRadius = 50.0f; 
	FVector ForwardStart = ActorLocation;
	FVector ForwardEnd = ActorLocation + (MoveDir * ObstacleCheckRange);

	bool bHitWall = GetWorld()->SweepSingleByChannel(
        WallHit, 
        ForwardStart, 
        ForwardEnd, 
        FQuat::Identity, 
        ECC_WorldStatic, 
        FCollisionShape::MakeSphere(SphereRadius), 
        Params
    );

	if (bHitWall)
	{
		// Add force away from the wall (ImpactNormal)
        // ImpactNormal is the vector pointing out from the wall surface
		AvoidanceVector += WallHit.ImpactNormal * AvoidanceForceMultiplier;
        
        // Debug
        // DrawDebugLine(GetWorld(), ActorLocation, WallHit.ImpactPoint, FColor::Red, false, 0.1f);
	}

	return AvoidanceVector;
}

void AEnemyFlyBase::PickNewHoverDirection()
{
	if (!IsAlive()) return;

	FVector NewDir = FMath::VRand();
	NewDir.Z *= 0.25f; // Flatten vertical movement

	if (HasValidTarget())
	{
		FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
		float Dist = ToTarget.Size();
		FVector DirToTarget = ToTarget.GetSafeNormal();

		if (Dist > PreferredMaxRange)
		{
			// Pull closer
			NewDir = (NewDir * 0.5f + DirToTarget).GetSafeNormal();
		}
		else if (Dist < PreferredMinRange)
		{
			// Push away
			NewDir = (NewDir * 0.5f - DirToTarget).GetSafeNormal();
		}
		else
		{
			// Orbit logic
			FVector OrbitDir = FVector::CrossProduct(DirToTarget, FVector::UpVector);
			if (FMath::RandBool()) OrbitDir *= -1.0f;
			NewDir = (OrbitDir + NewDir * 0.5f).GetSafeNormal();
		}
	}

	CurrentHoverDirection = NewDir;
}

