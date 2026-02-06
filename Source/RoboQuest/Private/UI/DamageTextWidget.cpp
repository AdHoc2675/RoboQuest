// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDamageTextWidget::PlayDamageText(float Damage, FVector InWorldLocation, bool bIsCritical)
{
	// 1. Set Value
	if (DamageText)
	{
		// Display as integer by removing decimals
		int32 DamageInt = FMath::RoundToInt(Damage);
		DamageText->SetText(FText::AsNumber(DamageInt));

		// Change color/scale on Critical hit (Example)
		if (bIsCritical)
		{
			DamageText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f))); // Red
			SetRenderScale(FVector2D(1.5f, 1.5f));
		}
		else
		{
			DamageText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))); // White
			SetRenderScale(FVector2D(1.0f, 1.0f));
		}
	}

	// 2. Set Location
	WorldLocation = InWorldLocation;
	InitialLocation = InWorldLocation;
	CurrentLifeTime = 0.0f;

	// Add to Viewport (Safety check, though usually added by caller)
	if (!IsInViewport())
	{
		AddToViewport();
	}
}

void UDamageTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentLifeTime += InDeltaTime;

	// Check lifetime expiration
	if (CurrentLifeTime >= Duration)
	{
		RemoveFromParent();
		return;
	}

	// Update World coordinates (Float upwards)
	// Rise along World Z-axis
	WorldLocation.Z += FloatSpeed * InDeltaTime;

	// Convert World -> Screen coordinates
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	FVector2D ScreenPosition;
	bool bProjected = UGameplayStatics::ProjectWorldToScreen(PC, WorldLocation, ScreenPosition);

	if (bProjected)
	{
		// Set Position in Viewport
		SetPositionInViewport(ScreenPosition);
		
		float FadeStart = Duration * 0.5f;
		if (CurrentLifeTime > FadeStart)
		{
			float Alpha = 1.0f - ((CurrentLifeTime - FadeStart) / (Duration - FadeStart));
			SetRenderOpacity(Alpha);
		}
	}
}

