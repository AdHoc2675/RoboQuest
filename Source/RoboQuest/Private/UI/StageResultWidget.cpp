// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StageResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UStageResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UStageResultWidget::OnContinueClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UStageResultWidget::OnQuitClicked);
	}

	if (IntroAnim)
	{
		PlayAnimation(IntroAnim);
	}
}

void UStageResultWidget::SetupResultData(FName StageName, float PlayTime, int32 DefeatedEnemies, FString Rank)
{
	// 1. Set Stage Name
	if (StageNameText)
	{
		StageNameText->SetText(FText::FromName(StageName));
	}

	// 2. Set Play Time
	if (PlayTimeText)
	{
		// Format Time: MM:SS
		int32 Minutes = FMath::FloorToInt(PlayTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(PlayTime) % 60;
		FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		PlayTimeText->SetText(FText::FromString(TimeStr));
	}

	// 3. Set Rank (Image)
	if (RankImage)
	{
		UTexture2D* TargetTexture = RankTextureDefault;

		if (Rank == "S") TargetTexture = RankTextureS;
		else if (Rank == "A") TargetTexture = RankTextureA;
		else if (Rank == "B") TargetTexture = RankTextureB;
		
		if (TargetTexture)
		{
			RankImage->SetBrushFromTexture(TargetTexture);
			RankImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			// Hide image if no texture found
			RankImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	// (Optional) Keep text update just in case, or hide it
	if (RankText)
	{
		RankText->SetVisibility(ESlateVisibility::Hidden); // Hide text if we rely on image
		// RankText->SetText(FText::FromString(Rank)); 
	}
}

void UStageResultWidget::OnContinueClicked()
{
	// Logic to restart or go next
	UE_LOG(LogTemp, Log, TEXT("StageResultWidget: Continue Clicked"));
    
    // Example: Restart Level
    FName CurrentLevel = FName(*UGameplayStatics::GetCurrentLevelName(this));
    UGameplayStatics::OpenLevel(this, CurrentLevel);
}

void UStageResultWidget::OnQuitClicked()
{
	// Logic to return to menu
	UE_LOG(LogTemp, Log, TEXT("StageResultWidget: Quit Clicked"));
    
    // Example: Open Main Menu
    // UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));
}

