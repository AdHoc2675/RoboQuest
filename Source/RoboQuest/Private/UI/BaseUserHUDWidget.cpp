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
