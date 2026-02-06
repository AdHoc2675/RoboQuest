// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponAffix.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Affix_DamageUp.generated.h"

/**
 * Example Affix: Increases Weapon Damage
 */
UCLASS()
class ROBOQUEST_API UAffix_DamageUp : public UWeaponAffix
{
	GENERATED_BODY()

public:
	UAffix_DamageUp()
	{
		AffixName = FText::FromString(TEXT("Sharp"));
		AffixDescription = FText::FromString(TEXT("Damage +20%"));
		AffixColor = FLinearColor(1.0f, 1.0f, 1.0f); // Light Red
	}

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float DamageBonusPercent = 0.2f;

	virtual void ApplyStatModifiers(UTP_WeaponComponent* Weapon) override
	{
		if (Weapon)
		{
			// Add 0.2 to the multiplier
			Weapon->DamageMultiplier += DamageBonusPercent;
			UE_LOG(LogTemp, Log, TEXT("Affix_DamageUp: New DamageMultiplier = %.2f"), Weapon->DamageMultiplier);
		}
	}
};
