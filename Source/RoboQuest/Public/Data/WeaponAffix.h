// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponAffix.generated.h"

class UTP_WeaponComponent;

/**
 * Base class for all Weapon Affixes (Perks/Runes).
 * Inherit from this to create specific effects (e.g., SharpAffix, ExplosiveAffix).
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ROBOQUEST_API UWeaponAffix : public UObject
{
	GENERATED_BODY()

public:
	// Display Name (e.g. "Sharp")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Affix")
	FText AffixName;

	// Description (e.g. "Increases Damage by 10%")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Affix")
	FText AffixDescription;

	// Color for UI display
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Affix")
	FLinearColor AffixColor = FLinearColor::White;

	// --- Virtual Functions for Behavior ---

	/** Called when the affix is applied to the weapon. Use to modify base stats. */
	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon);

	/** Called when the weapon fires (Optional hook) */
	virtual void OnFire(UTP_WeaponComponent* Weapon) {}

	// Helper to get all available random affixes
	static TArray<TSubclassOf<UWeaponAffix>> GetAllRandomAffixes();
	// Add more hooks as needed: OnReload, OnHit, etc.
};
