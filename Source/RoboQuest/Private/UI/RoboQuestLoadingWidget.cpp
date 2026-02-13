// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RoboQuestLoadingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/AmbientSound.h"
#include "System/RoboQuestGameInstance.h"

void URoboQuestLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Ensure widget can receive input (Important for NativeOnKeyDown)
	bIsFocusable = true;
}

void URoboQuestLoadingWidget::UpdatePresentation(const FLevelLoadingData& Data)
{
	// 1. Setup UI
	if (BackgroundImage && Data.BackgroundImage)
		BackgroundImage->SetBrushFromTexture(Data.BackgroundImage);

	if (LevelNameText)
		LevelNameText->SetText(Data.LevelDisplayName);

	if (TipTitleText)
		TipTitleText->SetText(Data.TipTitle);

	if (TipDescText)
		TipDescText->SetText(Data.TipDescription);

	// 2. Reset State
	bIsLoadingComplete = false;
	bHasProceeded = false;
	if (LoadingIndicatorPanel) LoadingIndicatorPanel->SetVisibility(ESlateVisibility::Visible);
	if (PressKeyPanel) PressKeyPanel->SetVisibility(ESlateVisibility::Hidden);

	TArray<AActor*> AmbientSounds;
	UGameplayStatics::GetAllActorsOfClass(this, AAmbientSound::StaticClass(), AmbientSounds);
	for (AActor* Actor : AmbientSounds)
	{
		if (AAmbientSound* Amb = Cast<AAmbientSound>(Actor))
		{
			if (UAudioComponent* Audio = Amb->GetAudioComponent())
			{
				// Fade out over 0.5 seconds
				Audio->FadeOut(0.5f, 0.0f);
			}
		}
	}

	if (LoadingBGM)
	{
		UGameplayStatics::PlaySound2D(this, LoadingBGM);
		
	}
	
	// Force focus to capture keyboard input
	// Using a slight delay or calling in Tick is sometimes safer, but this usually works if widget is in viewport
	SetKeyboardFocus();
}

void URoboQuestLoadingWidget::SetLoadingComplete()
{
	bIsLoadingComplete = true;

	// Swap UI
	if (LoadingIndicatorPanel) LoadingIndicatorPanel->SetVisibility(ESlateVisibility::Hidden);
	if (PressKeyPanel) PressKeyPanel->SetVisibility(ESlateVisibility::Visible);

	// Play Complete Sound
	if (LoadingCompleteSound)
	{
		// Spawn/Play sound securely
		UGameplayStatics::PlaySound2D(this, LoadingCompleteSound);
	}

	// Re-acquire focus simply to be safe
	SetKeyboardFocus();
}

FReply URoboQuestLoadingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// If loading is finished, any key triggers the transition
	if (bIsLoadingComplete)
	{
		ProceedToLevel();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply URoboQuestLoadingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// If loading is finished, mouse click proceeds too
	if (bIsLoadingComplete)
	{
		ProceedToLevel();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URoboQuestLoadingWidget::ProceedToLevel()
{
	if (bHasProceeded) return; // Prevent double calls
	bHasProceeded = true;

	// Notify GameInstance to actually switch levels
	if (URoboQuestGameInstance* GI = Cast<URoboQuestGameInstance>(GetGameInstance()))
	{
		GI->FinishLoading();
	}
}

void URoboQuestLoadingWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void URoboQuestLoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Accumulate time for Sin wave
	TotalTime += InDeltaTime;

	// Far Background Move (Slow)
	// Moves Left (-X)
	if (FarBackgroundImage)
	{
		float FarSpeed = -80.0f;
		FVector2D NewPos = FarBackgroundImage->GetRenderTransform().Translation;
		NewPos.X += FarSpeed * InDeltaTime;

		// Optional: Reset if it goes too far? (Depends on image size, usually just let it drift)
		// if (NewPos.X < -1000.0f) NewPos.X = 0.0f; 

		FarBackgroundImage->SetRenderTranslation(NewPos);
	}

	// Main Background Move (Faster Parallax)
	// Moves Left (-X)
	if (BackgroundImage)
	{
		float BgSpeed = -300.0f;
		FVector2D NewPos = BackgroundImage->GetRenderTransform().Translation;
		NewPos.X += BgSpeed * InDeltaTime;
		
		BackgroundImage->SetRenderTranslation(NewPos);
	}

	// Bus Bobbing (Up/Down)
	// Uses Sin wave physics: Amplitude * Sin(Time * Frequency)
	if (BusImage)
	{
		float Amplitude = 10.0f; // Pixel offset range
		float Frequency = 1.0f;  // Speed of bobbing

		float YOffset = FMath::Sin(TotalTime * Frequency) * Amplitude;

		// We only modify Y, keep current X (if any)
		FVector2D CurrentTrans = BusImage->GetRenderTransform().Translation;
		BusImage->SetRenderTranslation(FVector2D(CurrentTrans.X, YOffset));
	}
}

