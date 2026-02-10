// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/WeaponAffix.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Data/Affixes/Affix_DamageUp.h"
#include "Data/Affixes/WeaponAffixes_General.h" 

void UWeaponAffix::ApplyStatModifiers(UTP_WeaponComponent* Weapon)
{
	// Base implementation does nothing.
	// Override this to modify Weapon->DamageMultiplier, etc.
}

TArray<TSubclassOf<UWeaponAffix>> UWeaponAffix::GetAllRandomAffixes()
{
    static TArray<TSubclassOf<UWeaponAffix>> AffixList;
    if (AffixList.Num() == 0)
    {
        // Add all your created affix classes here
        AffixList.Add(UAffix_DamageUp::StaticClass());
        AffixList.Add(UAffix_Swift::StaticClass());
        AffixList.Add(UAffix_BigMag::StaticClass());
        AffixList.Add(UAffix_Cadence::StaticClass());
        AffixList.Add(UAffix_Longshot::StaticClass());
        AffixList.Add(UAffix_Quickload::StaticClass());
        AffixList.Add(UAffix_Burn::StaticClass());
        AffixList.Add(UAffix_Cryo::StaticClass());
        AffixList.Add(UAffix_Shock::StaticClass());
    }
    return AffixList;
}