// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/EnemyBase.h"
#include "EnemySpawnPoint.generated.h"

/**
 * Actor used to mark locations where enemies will spawn inside a Combat Zone.
 * Visually represented in the editor but invisible in-game.
 */
UCLASS()
class ROBOQUEST_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawnPoint();

    // The type of enemy to spawn at this specific point.
    // Can be left empty if the CombatZone controls the enemy type.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Settings")
	TSubclassOf<AEnemyBase> EnemyClassToSpawn;

    // Helper component to visualize the direction in the editor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UArrowComponent* ArrowComponent;

public:
    // Spawns the enemy immediately without any effects
    AEnemyBase* SpawnEnemy();
};
