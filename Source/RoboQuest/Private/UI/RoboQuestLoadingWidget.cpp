// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/RoboQuestLoadingWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URoboQuestLoadingWidget::UpdatePresentation(const FLevelLoadingData& Data)
{
	// Set Background
	if (BackgroundImage && Data.BackgroundImage)
	{
		BackgroundImage->SetBrushFromTexture(Data.BackgroundImage);
	}

	// Set Level Name
	if (LevelNameText)
	{
		LevelNameText->SetText(Data.LevelDisplayName);
	}

	// Set Tip Info
	if (TipTitleText)
	{
		TipTitleText->SetText(Data.TipTitle);
	}

	if (TipDescText)
	{
		TipDescText->SetText(Data.TipDescription);
	}
}

