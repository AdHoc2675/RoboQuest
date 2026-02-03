// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h" // Assuming IInteractable interface is defined here
#include "WeaponUpgradeStation.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * An interactive station that upgrades the player's weapon.
 * Requires holding the Interact key (E) for a specific duration.
 * Costs 'Power' currency equivalent to the current weapon level.
 */
UCLASS()
class ROBOQUEST_API AWeaponUpgradeStation : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponUpgradeStation();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

    // Text to display cost or status above the station
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* InfoTextRender;

	// --- Config ---
    // Time required to hold interact to trigger upgrade (Seconds)
    UPROPERTY(EditAnywhere, Category = "Upgrade")
    float RequiredHoldTime = 1.0f;

public:
    // Interface implementation
    virtual void Interact_Implementation(AActor* Interactor) override;

private:
    // Tracks how long the player has been holding interact
    float CurrentHoldTime = 0.0f;

    // Timestamp of the last interaction to detect when button is released
    double LastInteractTime = 0.0;

    // Perform the actual upgrade logic
    void ProcessUpgrade(class ARoboQuestCharacter* PlayerChar);

    // Update the 3D text (e.g. "Hold E to Upgrade")
    void UpdateInfoText(int32 CurrentCost, int32 PlayerPower);
};
