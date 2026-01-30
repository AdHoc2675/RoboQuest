// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CrosshairWidget.h"
#include "Components/Border.h"

void UCrosshairWidget::UpdateSpread(float CurrentSpread)
{
	// Calculate pixel offset based on spread
	float Offset = CurrentSpread * SpreadScale;

	// Clamp to prevent it from going too wide if needed, or leave dynamic
	// Offset = FMath::Clamp(Offset, 0.0f, 200.0f);

	if (Crosshair_Top)
	{
		Crosshair_Top->SetRenderTranslation(FVector2D(0.0f, -Offset));
	}

	if (Crosshair_Bottom)
	{
		Crosshair_Bottom->SetRenderTranslation(FVector2D(0.0f, Offset));
	}

	if (Crosshair_Left)
	{
		Crosshair_Left->SetRenderTranslation(FVector2D(-Offset, 0.0f));
	}

	if (Crosshair_Right)
	{
		Crosshair_Right->SetRenderTranslation(FVector2D(Offset, 0.0f));
	}
}
