// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WeaponDetailWidget.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UWeaponDetailWidget::UpdateWeaponDetails(UTP_WeaponComponent* WeaponComp)
{
    if (!WeaponComp) return;

    // 1. Header Info
    if (Text_WeaponName)
    {
        // Use RowName or a display name lookup
        Text_WeaponName->SetText(FText::FromName(WeaponComp->WeaponRowName));
    }

    // 2. Level & Rarity
    if (Text_WeaponLevel)
    {
        Text_WeaponLevel->SetText(FText::Format(NSLOCTEXT("UI", "LevelFmt", "Level {0}"), FText::AsNumber(WeaponComp->WeaponLevel)));
    }
    
    if (Text_Rarity)
    {
        Text_Rarity->SetText(GetRarityText(WeaponComp->WeaponRarity));
        Text_Rarity->SetColorAndOpacity(FSlateColor(GetRarityColor(WeaponComp->WeaponRarity)));
        // Note: You can also bind proper stars (images) here if you add UImage bindings.
    }

    // 3. Stats Grid
    if (Text_Value_Damage)
    {
        // One decimal place (e.g. 30.8)
        Text_Value_Damage->SetText(FText::AsNumber(WeaponComp->FinalDamage, &FNumberFormattingOptions::DefaultWithGrouping()));
    }

    if (Text_Value_FireRate)
    {
        // Display as "6.67/s"
        FString RateStr = FString::Printf(TEXT("%.2f/s"), WeaponComp->RateOfFire);
        Text_Value_FireRate->SetText(FText::FromString(RateStr));
    }

    if (Text_Value_Range)
    {
        // Assume RangeMeter is the effective range or radius. "100.0m"
        FString RangeStr = FString::Printf(TEXT("%.1fm"), WeaponComp->FinalRangeMeter);
        Text_Value_Range->SetText(FText::FromString(RangeStr));
    }

    if (Text_Value_Crit)
    {
        // "x1.5"
        FString CritStr = FString::Printf(TEXT("x%.1f"), WeaponComp->CritDamageMultiplier);
        Text_Value_Crit->SetText(FText::FromString(CritStr));
    }

    if (Text_Value_AmmoType)
    {
        FString TypeStr = (WeaponComp->AmmoType == EAmmoType::Energy) ? TEXT("Energy") : TEXT("Magazine");
        Text_Value_AmmoType->SetText(FText::FromString(TypeStr));
    }

    // 4. Affixes (Mock data population as WeaponComponent has no perks yet)
    if (VBox_AffixList)
    {
        VBox_AffixList->ClearChildren();

        // [Mock] Add dynamic stats based on level
        if (WeaponComp->WeaponLevel > 0)
        {
            // Calculate amplification (e.g. 10% Damage and 5% Range per level)
            const int32 DamageAmp = WeaponComp->WeaponLevel * 10;
            const int32 RangeAmp = WeaponComp->WeaponLevel * 10;

            FString LevelStr = FString::Printf(TEXT("Level %d: Damage +%d%% & Range +%d%%"), 
                WeaponComp->WeaponLevel, DamageAmp, RangeAmp);

            AddAffixRow(LevelStr, FLinearColor(0.8f, 0.8f, 0.8f)); // Greyish
        }
        
        // [Mock] Add a trait based on Weapon Name
        if (WeaponComp->WeaponRowName.ToString().Contains("Torpedo"))
        {
            AddAffixRow(TEXT("Explosive: Projectiles explode on impact"), FLinearColor(1.0f, 0.8f, 0.2f)); // Orange
            AddAffixRow(TEXT("Homing: Projectiles seek targets"), FLinearColor::White);
        }
    }
}

void UWeaponDetailWidget::AddAffixRow(FString AffixText, FLinearColor Color)
{
    if (!VBox_AffixList) return;

    // Create a new TextBlock dynamically
    UTextBlock* NewBlock = NewObject<UTextBlock>(this);
    if (NewBlock)
    {
        NewBlock->SetText(FText::FromString(TEXT("| ") + AffixText));
        NewBlock->SetColorAndOpacity(FSlateColor(Color));
        NewBlock->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 16)); // Example font
        
        VBox_AffixList->AddChild(NewBlock);
    }
}

FText UWeaponDetailWidget::GetRarityText(EWeaponRarity Rarity)
{
    switch (Rarity)
    {
    case EWeaponRarity::Common:     return FText::FromString(TEXT("Common"));
    case EWeaponRarity::Uncommon:   return FText::FromString(TEXT("Uncommon"));
    case EWeaponRarity::Rare:       return FText::FromString(TEXT("Rare"));
    case EWeaponRarity::Epic:       return FText::FromString(TEXT("Epic"));
    case EWeaponRarity::Fantastic:  return FText::FromString(TEXT("Fantastic"));
    default:                        return FText::FromString(TEXT("Common"));
    }
}

FLinearColor UWeaponDetailWidget::GetRarityColor(EWeaponRarity Rarity)
{
    switch (Rarity)
    {
    case EWeaponRarity::Common:     return FLinearColor(1.0f, 1.0f, 1.0f); // white
    case EWeaponRarity::Uncommon:   return FLinearColor(0.2f, 1.0f, 0.2f); // Green
    case EWeaponRarity::Rare:       return FLinearColor(0.2f, 0.6f, 1.0f); // Blue
    case EWeaponRarity::Epic:       return FLinearColor(0.6f, 0.2f, 1.0f); // Purple
    case EWeaponRarity::Fantastic:  return FLinearColor(1.0f, 0.5f, 0.0f); // Gold
    default:                        return FLinearColor::White;
    }
}
