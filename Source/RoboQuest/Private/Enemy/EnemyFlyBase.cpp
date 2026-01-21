// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFlyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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
	bUseControllerRotationYaw = false; // Don't snap to controller
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

