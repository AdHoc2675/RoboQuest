// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AbilityDisplayWidget.h"
#include "Abilities/RoboQuestAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAbilityDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Check binding state on startup
	if (!CooldownText)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AbilityDisplayWidget] CooldownText is NULL! Check Widget Blueprint Name."));
	}
	else
	{
		CooldownText->SetVisibility(ESlateVisibility::Hidden);
	}

    // Check StackText binding
    if (StackText)
    {
        StackText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UAbilityDisplayWidget::BindAbility(URoboQuestAbility* NewAbility)
{
    // 1. Assign Ability
	BoundAbility = NewAbility;

    // 2. Validate and Log
	if (BoundAbility.IsValid())
	{
        UE_LOG(LogTemp, Warning, TEXT("[AbilityDisplayWidget] Successfully Bound Ability: %s"), *BoundAbility->GetName());

		// Set Icon if valid
		if (AbilityIcon && BoundAbility->Icon)
		{
			AbilityIcon->SetBrushFromTexture(BoundAbility->Icon);
            AbilityIcon->SetColorAndOpacity(FLinearColor::White); // Reset color
		}

		// Notify BP
		OnAbilityBound();
	}
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AbilityDisplayWidget] BindAbility called with NULL Ability!"));
    }
}

void UAbilityDisplayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    // Stop if no ability
	if (!BoundAbility.IsValid())
	{
		return;
	}

    // Update Stack Display
    if (BoundAbility->HasStacks())
    {
        if (StackText)
        {
            int32 Stacks = BoundAbility->GetCurrentStackCount();
            
            // Only show if stacks > 0 (Optional preference, usually better to zero-hide or show 0)
            if (Stacks >= 0)
            {
                StackText->SetVisibility(ESlateVisibility::HitTestInvisible);
                StackText->SetText(FText::AsNumber(Stacks));
            }
            else
            {
                // Hide if 0
                StackText->SetVisibility(ESlateVisibility::Hidden);
                
                // Or Show 0 (If preferred, uncomment below and comment above)
                // StackText->SetVisibility(ESlateVisibility::HitTestInvisible);
                // StackText->SetText(FText::AsNumber(0));
            }
        }
    }
    else
    {
        // Hide if ability doesn't support stacks
        if (StackText)
        {
            StackText->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Check cooldown state
	if (BoundAbility->IsOnCooldown())
	{
        // --- COOLDOWN STATE ---
		float TimeLeft = BoundAbility->GetCooldownTimeRemaining();

        // Update Text
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::HitTestInvisible);
            
            // Format text based on time remaining
            if (TimeLeft >= 1.0f)
            {
                // Show as integer rounded up (e.g., "3", "2", "1")
                CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(TimeLeft))); 
            }
            else
            {
                // Show tenths of a second (e.g., "0.9", "0.5")
                FNumberFormattingOptions NumberFormat;
                NumberFormat.MinimumIntegralDigits = 1;
                NumberFormat.MaximumIntegralDigits = 1;
                NumberFormat.MinimumFractionalDigits = 1;
                NumberFormat.MaximumFractionalDigits = 1;
                
                CooldownText->SetText(FText::AsNumber(TimeLeft, &NumberFormat)); 
            }
		}

        // Darken Icon
		if (AbilityIcon)
		{
			AbilityIcon->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
		}
	}
	else
	{
        // --- READY STATE ---
        
        // Hide Text
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}

        // Restore Icon
		if (AbilityIcon)
		{
			AbilityIcon->SetColorAndOpacity(FLinearColor::White);
		}
	}
}