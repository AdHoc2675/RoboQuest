// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/EnemySpawnPoint.h"
#include "Interactable/SlidingDoor.h"
#include "CombatZone.generated.h"

class UBoxComponent;

/**
 * Manages a combat area.
 * Triggers enemy spawns immediately when the player enters the volume.
 */
UCLASS()
class ROBOQUEST_API ACombatZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ACombatZone();

protected:
    // Trigger volume to activate the combat zone
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* TriggerBox;

    // List of spawn points linked to this zone
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Combat Settings")
    TArray<AEnemySpawnPoint*> SpawnPoints;

	// Doors to lock / unlock (Using ASlidingDoor now)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Combat Settings")
    TArray<ASlidingDoor*> LinkedDoors;

	// Doors to open when completed
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Combat Settings")
    TArray<ASlidingDoor*> DoorsToOpenOnComplete;

    bool bIsActive;
    bool bIsCompleted = false;
    int32 AliveEnemyCount = 0;

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Spawns all enemies for this zone instantly
    void ActivateZone();

    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* DeadEnemy);

    void CompleteZone();
};
