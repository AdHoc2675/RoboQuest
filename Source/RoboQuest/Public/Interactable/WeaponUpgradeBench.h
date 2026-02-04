// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WeaponUpgradeBench.generated.h"

class UStaticMeshComponent;

/**
 * A robust weapon upgrade station logic.
 * Consumes Power Cells based on the weapon's current level to upgrade stats.
 */
UCLASS()
class ROBOQUEST_API AWeaponUpgradeBench : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponUpgradeBench();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame to handle text reset logic
	virtual void Tick(float DeltaTime) override;

    // Interface implementation
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual bool ShouldShowWeaponDetail_Implementation() override;

	virtual FText GetInteractionPrompt_Implementation() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BenchMesh;

	// Sound to play when upgrade is successful
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* UpgradeSuccessSound;

    // Sound to play when upgrade fails
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* UpgradeFailSound;

private:
	// Time when the interaction last occurred, used to reset text
	double LastInteractTime = -10.0;
    
    // Cached reference to the interacting player to clear the HUD message later
    UPROPERTY()
    class ARoboQuestCharacter* LastInteractorCharacter;

	// Helper to update the text display on the Player's HUD
	void UpdateHUDMessage(class ARoboQuestCharacter* Player, FString NewText, FLinearColor Color);
};
