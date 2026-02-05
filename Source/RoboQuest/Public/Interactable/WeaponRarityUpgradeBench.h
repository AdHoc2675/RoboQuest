// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Data/WeaponStatRow.h" // For EWeaponRarity
#include "WeaponRarityUpgradeBench.generated.h"

class UStaticMeshComponent;
class ARoboQuestCharacter;

/**
 * An interaction station that upgrades the weapon's Rarity.
 * Costs follow the sequence: 4 -> 6 -> 8 -> 10 Power Cells.
 */
UCLASS()
class ROBOQUEST_API AWeaponRarityUpgradeBench : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponRarityUpgradeBench();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// --- IInteractable Interface ---
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() override;
	virtual bool ShouldShowWeaponDetail_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BenchMesh;

    // Index of the material element to change
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    int32 EmissionMaterialIndex = 1;

    // Parameter name for Emission (Glow)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FName EmissiveColorParamName = TEXT("EmissiveColor");

    // [New] Parameter name for Base Color (Albedo)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FName BaseColorParamName = TEXT("BaseColor");

    // [New] Intensity multiplier for the glow. Lower this if it's too white.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float EmissionIntensity = 5.0f; 

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

	// Sound to play when upgrade is successful
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* UpgradeSuccessSound;

	// Sound to play when upgrade fails
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* UpgradeFailSound;

private:
	// Time when the interaction last occurred, used to reset HUD text
	double LastInteractTime = -10.0;

	// Cached reference to the interacting player to clear the HUD message later
	UPROPERTY()
	ARoboQuestCharacter* LastInteractorCharacter;

	// Helper to calculate upgrade cost based on current rarity
	// Returns -1 if max rarity
	int32 GetUpgradeCost(EWeaponRarity CurrentRarity) const;

	// Helper to update the text display on the Player's HUD
	void UpdateHUDMessage(ARoboQuestCharacter* Player, FString NewText, FLinearColor Color);

	// Helper to get color from rarity
	FLinearColor GetRarityColor(EWeaponRarity Rarity) const;

	// Update the mesh emission color
	void UpdateBenchColor(ARoboQuestCharacter* Player);
};
