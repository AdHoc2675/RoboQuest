// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h" 
#include "WeaponUpgradeStation.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * An interactive station that upgrades the player's weapon instantly upon interaction.
 * Costs 'Power' currency equivalent to the current weapon level.
 */
UCLASS()
class ROBOQUEST_API AWeaponUpgradeStation : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponUpgradeStation();
    // Tick is no longer strictly needed for logic, but can be kept for simple text animations
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* InfoTextRender;

public:
    // Interface implementation
    virtual void Interact_Implementation(AActor* Interactor) override;

private:
    double LastInteractTimer = 0.0f; // For temporary text display

    // Perform the actual upgrade logic
    void ProcessUpgrade(class ARoboQuestCharacter* PlayerChar);

    // Update the 3D text
    void UpdateInfoText(FString NewText);
};
