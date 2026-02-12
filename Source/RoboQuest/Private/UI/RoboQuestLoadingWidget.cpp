// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RoboQuestLoadingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/RoboQuestGameInstance.h"

void URoboQuestLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Ensure widget can receive input
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
	if (LoadingIndicatorPanel) LoadingIndicatorPanel->SetVisibility(ESlateVisibility::Visible);
	if (PressKeyPanel) PressKeyPanel->SetVisibility(ESlateVisibility::Hidden);

	// 3. Play BGM
	if (LoadingBGM)
	{
		if (!BGMComponent)
		{
			BGMComponent = UGameplayStatics::SpawnSound2D(this, LoadingBGM, 1.0f, 1.0f, 0.0f, nullptr, true, true);
		}
	}
	
	// Set Focus to capture keyboard input later
	SetKeyboardFocus();
}

void URoboQuestLoadingWidget::SetLoadingComplete()
{
	bIsLoadingComplete = true;

	// 1. Swap UI
	if (LoadingIndicatorPanel) LoadingIndicatorPanel->SetVisibility(ESlateVisibility::Hidden);
	if (PressKeyPanel) PressKeyPanel->SetVisibility(ESlateVisibility::Visible);

	// 2. Play Complete Sound
	if (LoadingCompleteSound)
	{
		UGameplayStatics::PlaySound2D(this, LoadingCompleteSound);
	}
}

FReply URoboQuestLoadingWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsLoadingComplete)
	{
		ProceedToLevel();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply URoboQuestLoadingWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsLoadingComplete)
	{
		ProceedToLevel();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URoboQuestLoadingWidget::ProceedToLevel()
{
	// Stop BGM
	if (BGMComponent)
	{
		BGMComponent->Stop();
	}

	// Notify GameInstance to actually switch levels
	if (URoboQuestGameInstance* GI = Cast<URoboQuestGameInstance>(GetGameInstance()))
	{
		GI->FinishLoading();
	}
}

void URoboQuestLoadingWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (BGMComponent)
	{
		BGMComponent->Stop();
	}
}

