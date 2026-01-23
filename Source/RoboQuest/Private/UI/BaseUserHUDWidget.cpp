// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseUserHUDWidget.h"

void UBaseUserHUDWidget::UpdateHealthState(float CurrentHP, float ScratchHP, float MaxHP)
{
    if (MaxHP <= 0.0f)
    {
        MaxHP = 1.0f;
    }

	// calculate health ratio (0.0 ~ 1.0)
    float HealthPercent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);

	// calculate scratch ratio (0.0 ~ 1.0)
    float ScratchPercent = FMath::Clamp(ScratchHP / MaxHP, 0.0f, 1.0f);

	// update UI elements
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercent);
    }

    if (HealthScratchBar)
    {
        HealthScratchBar->SetPercent(FMath::Max(HealthPercent, ScratchPercent));
    }

    if (HPText)
    {
        // e.g., display as "50 / 100"
        HPText->SetText(FText::Format(NSLOCTEXT("HUD", "HPFormat", "{0} / {1}"), 
            FText::AsNumber(FMath::CeilToInt(CurrentHP)), 
            FText::AsNumber(FMath::CeilToInt(MaxHP))));
    }
}

void UBaseUserHUDWidget::UpdateAmmoState(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (CurrentBulletText)
    {
        CurrentBulletText->SetText(FText::AsNumber(CurrentAmmo));
    }

    if (MaxBulletText)
    {
        MaxBulletText->SetText(FText::AsNumber(MaxAmmo));
    }
}

void UBaseUserHUDWidget::UpdateExpState(float CurrentExp, float MaxExp, int32 CurrentLevel)
{
    if (MaxExp <= 0.0f)
    {
        MaxExp = 1.0f;
    }

    // calculate exp ratio (0.0 ~ 1.0)
    float ExpPercent = FMath::Clamp(CurrentExp / MaxExp, 0.0f, 1.0f);
    // update UI elements
    if (EXPBar)
    {
        EXPBar->SetPercent(ExpPercent);
    }

    if (LevelText)
    {
        // Lv: X
        LevelText->SetText(FText::Format(NSLOCTEXT("HUD", "LevelFormat", "Lv {0}"), FText::AsNumber(CurrentLevel)));
	}
}

void UBaseUserHUDWidget::UpdatePlayerStats(float DefensePercent, float SpeedMultiplier)
{
	if (ShieldText)
	{
		// Display as percentage. e.g., 0.15 -> "Def: 15%"
        int32 DefInt = FMath::RoundToInt(DefensePercent * 100.0f);
		ShieldText->SetText(FText::Format(NSLOCTEXT("HUD", "DefFormat", "Def: {0}%"), FText::AsNumber(DefInt)));
	}

	if (SpeedText)
	{
		// Display as percentage change. e.g., 1.2 -> "Spd: +20%", 1.0 -> "Spd: +0%"
        int32 SpdInt = FMath::RoundToInt((SpeedMultiplier - 1.0f) * 100.0f);
        
        // Add '+' sign for positive changes
        FString Sign = (SpdInt >= 0) ? TEXT("+") : TEXT(""); 
        
		SpeedText->SetText(FText::Format(NSLOCTEXT("HUD", "SpdFormat", "Spd: {0}{1}%"), FText::FromString(Sign), FText::AsNumber(SpdInt)));
	}
}