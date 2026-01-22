// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyPawnBase.h"
#include "Enemy/EnemyPawnAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

AEnemyPawnBase::AEnemyPawnBase()
{
	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;

	// Configure Character Movement for walking
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DefaultLandMovementMode = MOVE_Walking;
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}

	// By default we face the target (strafing style)
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationYaw = false;

	// Default strafe
	StrafeDirectionScale = 0.0f;

	// Set default AI Controller
	AIControllerClass = AEnemyPawnAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyPawnBase::BeginPlay()
{
	Super::BeginPlay();

	// Start the strafing logic loop
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(StrafeTimerHandle, this, &AEnemyPawnBase::PickNewStrafeDirection, StrafeChangeInterval, true);
	}
}

void AEnemyPawnBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(StrafeTimerHandle);
	}
}

void AEnemyPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	FindTarget();

	if (HasValidTarget())
	{
		// Always face target
		RotateTowardsTarget(DeltaTime);

		// Movement Logic Logic:
		// We only Manual Move (CombatMove) if we are close enough and have Line of Sight.
		// Otherwise, the AIController handles the pathfinding approach.

		float Dist = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
		bool bCanSee = CanSeeTarget(); // Assuming you have this function exposed or check it here

		// This threshold logic matches the AIController's switch logic
		if (bCanSee && Dist <= PreferredMaxRange * 1.5f)
		{
			CombatMove(DeltaTime);
		}
	}
}

void AEnemyPawnBase::FindTarget()
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
	else
	{
		CurrentTarget = nullptr;
	}
}

void AEnemyPawnBase::RotateTowardsTarget(float DeltaTime)
{
	if (!CurrentTarget) return;

	FVector Start = GetActorLocation();
	FVector End = CurrentTarget->GetActorLocation();

	// Look at target but lock pitch/roll (grounded)
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Start, End);
	LookAtRot.Pitch = 0.0f;
	LookAtRot.Roll = 0.0f;

	FRotator NewRot = FMath::RInterpTo(GetActorRotation(), LookAtRot, DeltaTime, RotationSpeed);
	SetActorRotation(NewRot);
}

void AEnemyPawnBase::CombatMove(float DeltaTime)
{
	if (!CurrentTarget) return;

	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = CurrentTarget->GetActorLocation();

	float Dist = FVector::Dist(MyLoc, TargetLoc);

	// Calculate flat direction to target
	FVector ToTarget = (TargetLoc - MyLoc);
	ToTarget.Z = 0.0f;
	if (!ToTarget.IsNearlyZero())
	{
		ToTarget.Normalize();
	}
	else
	{
		ToTarget = GetActorForwardVector();
		ToTarget.Z = 0.0f;
		ToTarget.Normalize();
	}

	FVector MoveDirection = FVector::ZeroVector;

	// Range management: move forward/back to maintain preferred range
	if (Dist > PreferredMaxRange)
	{
		// Too far: move toward target
		MoveDirection += ToTarget;
	}
	else if (Dist < PreferredMinRange)
	{
		// Too close: back away from target
		MoveDirection -= ToTarget;
	}
	// If within preferred band, no forward/back movement

	// Strafing: add lateral movement based on current strafe scale
	if (StrafeDirectionScale != 0.0f)
	{
		FVector RightVector = FVector::CrossProduct(ToTarget, FVector::UpVector);
		RightVector.Z = 0.0f;
		if (!RightVector.IsNearlyZero())
		{
			RightVector.Normalize();
			MoveDirection += RightVector * StrafeDirectionScale;
		}
	}

	// Prevent zero vector add; normalize to avoid faster diagonal movement
	if (!MoveDirection.IsNearlyZero())
	{
		MoveDirection.Normalize();
		AddMovementInput(MoveDirection, StrafeSpeed);
	}
	else
	{
		// Optionally, stop movement input to hold position
		// No explicit stop necessary; CharacterMovement will decelerate
	}
}

void AEnemyPawnBase::MoveToTarget()
{
	// Fallback direct movement (not using NavMesh)
	if (!CurrentTarget) return;

	FVector Direction = (CurrentTarget->GetActorLocation() - GetActorLocation());
	Direction.Z = 0.0f;
	if (!Direction.IsNearlyZero())
	{
		Direction.Normalize();
		AddMovementInput(Direction);
	}
}

void AEnemyPawnBase::PickNewStrafeDirection()
{
	// Randomly choose -1 (left), 0 (none), 1 (right)
	int32 Rand = FMath::RandRange(-1, 1);
	StrafeDirectionScale = static_cast<float>(Rand);

	// Optional bias: prefer movement over standstill
	// if (Rand == 0 && FMath::RandBool()) StrafeDirectionScale = 1.0f;
}

bool AEnemyPawnBase::HasValidTarget() const
{
	return (CurrentTarget != nullptr);
}

bool AEnemyPawnBase::CanSeeTarget() const
{
	if (!CurrentTarget) return false;

	FHitResult Hit;
	FVector Start = GetActorLocation() + FVector(0, 0, 50.0f);
	FVector End = CurrentTarget->GetActorLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentTarget);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	return !bHit;
}