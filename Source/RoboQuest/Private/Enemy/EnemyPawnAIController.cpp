// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyPawnAIController.h"
#include "Enemy/EnemyPawnBase.h"

AEnemyPawnAIController::AEnemyPawnAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyPawnAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AEnemyPawnAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AEnemyPawnBase* EnemyPawn = Cast<AEnemyPawnBase>(GetPawn());
	if (EnemyPawn && EnemyPawn->IsAlive())
	{
		AActor* Target = EnemyPawn->GetTarget();
		if (Target)
		{
			float Dist = FVector::Dist(EnemyPawn->GetActorLocation(), Target->GetActorLocation());
			
			// If target is far away or not visible, use NavMesh pathfinding
			// DetectRange (1500) within which we engage directly
			// But if walls are in between, we need pathfinding even if close.
			bool bCanSee = EnemyPawn->CanSeeTarget();

			if (!bCanSee || Dist > EnemyPawn->PreferredMaxRange * 1.5f) 
			{
				// Far away or blocked: Use NavMesh to get closer
				MoveToActor(Target, EnemyPawn->PreferredMaxRange);
			}
			else
			{
				// Close and visible: Stop NavMesh movement and let Pawn handle CombatMove
				StopMovement();
			}
		}
	}
}