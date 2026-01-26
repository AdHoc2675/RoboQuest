// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemySpawnPoint.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"

// Sets default values
AEnemySpawnPoint::AEnemySpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Create an arrow component to visualize facing direction in the editor
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComponent->SetupAttachment(RootComponent);
    ArrowComponent->ArrowSize = 1.0f;
    ArrowComponent->bIsScreenSizeScaled = true;
}

AEnemyBase* AEnemySpawnPoint::SpawnEnemy()
{
    if (!EnemyClassToSpawn)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // Instant spawn at this actor's location and rotation
    AEnemyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClassToSpawn, GetActorLocation(), GetActorRotation(), SpawnParams);

    return SpawnedEnemy;
}

