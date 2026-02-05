// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponAffix.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "WeaponAffixes_General.generated.h"

// ----------------------------------------------------------------------
// 1. Swift: 10% movement speed increase while in hand
// ----------------------------------------------------------------------
UCLASS()
class ROBOQUEST_API UAffix_Swift : public UWeaponAffix
{
	GENERATED_BODY()
public:
	UAffix_Swift()
	{
		AffixName = FText::FromString(TEXT("Swift"));
		AffixDescription = FText::FromString(TEXT("Move Speed +10%"));
		AffixColor = FLinearColor(0.2f, 1.0f, 0.8f); // Cyan
	}

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
			Weapon->CurrentSpeedBonus += 0.1f;
		}
	}
};

// ----------------------------------------------------------------------
// 2. Big Mag: +50% magazine size
// ----------------------------------------------------------------------
UCLASS()
class ROBOQUEST_API UAffix_BigMag : public UWeaponAffix
{
	GENERATED_BODY()
public:
	UAffix_BigMag()
	{
		AffixName = FText::FromString(TEXT("Big Mag"));
		AffixDescription = FText::FromString(TEXT("Magazine Size +50%"));
		AffixColor = FLinearColor(0.2f, 0.5f, 1.0f); // Blue
	}

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
			Weapon->MaxAmmoMultiplier += 0.5f;
		}
	}
};

// ----------------------------------------------------------------------
// 3. Cadence: +15% firerate but reduces accuracy (Spread +20%)
// ----------------------------------------------------------------------
UCLASS()
class ROBOQUEST_API UAffix_Cadence : public UWeaponAffix
{
	GENERATED_BODY()
public:
	UAffix_Cadence()
	{
		AffixName = FText::FromString(TEXT("Cadence"));
		AffixDescription = FText::FromString(TEXT("Fire Rate +15% / Accuracy -20%"));
		AffixColor = FLinearColor(1.0f, 0.5f, 0.2f); // Orange
	}

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
			Weapon->RateOfFireMultiplier += 0.15f;
            // Higher Spread Multiplier = More Spread = Less Accuracy
			Weapon->SpreadMultiplier += 0.2f; 
		}
	}
};

// ----------------------------------------------------------------------
// 4. Longshot: +35% accuracy and +35% range
// ----------------------------------------------------------------------
UCLASS()
class ROBOQUEST_API UAffix_Longshot : public UWeaponAffix
{
	GENERATED_BODY()
public:
	UAffix_Longshot()
	{
		AffixName = FText::FromString(TEXT("Longshot"));
		AffixDescription = FText::FromString(TEXT("Accuracy +35% / Range +35%"));
		AffixColor = FLinearColor(0.2f, 0.8f, 0.2f); // Green
	}

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
            // Lower Spread = Higher Accuracy
			Weapon->SpreadMultiplier -= 0.35f; 
            // Clamp to prevent negative spread (perfect accuracy limit)
            if (Weapon->SpreadMultiplier < 0.0f) Weapon->SpreadMultiplier = 0.0f;

			Weapon->RangeMultiplier += 0.35f;
		}
	}
};

// ----------------------------------------------------------------------
// 5. Quickload: +20% reload speed
// ----------------------------------------------------------------------
UCLASS()
class ROBOQUEST_API UAffix_Quickload : public UWeaponAffix
{
	GENERATED_BODY()
public:
	UAffix_Quickload()
	{
		AffixName = FText::FromString(TEXT("Quickload"));
		AffixDescription = FText::FromString(TEXT("Reload Speed +20%"));
		AffixColor = FLinearColor(1.0f, 1.0f, 0.2f); // Yellow
	}

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
            // Speed +20% -> Time = Base / 1.2
			Weapon->ReloadTimeMultiplier += 0.2f;
		}
	}
};
