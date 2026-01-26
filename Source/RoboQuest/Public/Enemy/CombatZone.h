// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/EnemySpawnPoint.h"
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

    // Has this zone already been triggered?
    bool bIsActive;

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Spawns all enemies for this zone instantly
    void ActivateZone();
};
