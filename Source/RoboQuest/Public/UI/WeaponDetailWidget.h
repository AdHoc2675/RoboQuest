// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Data/WeaponStatRow.h"
#include "WeaponDetailWidget.generated.h"

class UTP_WeaponComponent;

/**
 * Widget to display detailed weapon stats, matching the graphical "Item Card" style.
 */
UCLASS()
class ROBOQUEST_API UWeaponDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // --- Header Section ---
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_WeaponName;

    // --- Rarity Bar Section ---
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_WeaponLevel;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Text_Rarity;

    // --- Stats Grid Section ---
    // 1. Damage
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Value_Damage;

    // 2. Fire Rate
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Value_FireRate;

    // 3. Range / Radius
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Value_Range; 

    // 4. Critical
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Value_Crit;

    // 5. Ammo Type
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_Value_AmmoType;

    // --- Body / Affixes Section ---
    // List container for dynamic perks (e.g. "| Explosive: Projectile explodes")
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* VBox_AffixList;

    /** Main update function */
    UFUNCTION(BlueprintCallable, Category = "UI|Weapon")
    void UpdateWeaponDetails(UTP_WeaponComponent* WeaponComp);

protected:
    // Helper to generate Affix text blocks
    void AddAffixRow(FString AffixText, FLinearColor Color = FLinearColor::White);
    
    // Helper to Determine Rarity Text based on Level (Mock logic)
    FText GetRarityText(EWeaponRarity Rarity);
    FLinearColor GetRarityColor(EWeaponRarity Rarity);
};
