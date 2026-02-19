// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StageResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
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
	if (StageNameText)
	{
		StageNameText->SetText(FText::FromName(StageName));
	}

	if (PlayTimeText)
	{
		// Format Time: MM:SS
		int32 Minutes = FMath::FloorToInt(PlayTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(PlayTime) % 60;
		FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		PlayTimeText->SetText(FText::FromString(TimeStr));
	}

	if (RankText)
	{
		RankText->SetText(FText::FromString(Rank));
	}
	
	// Ideally, calculate Rank based on PlayTime and DefeatedEnemies logic here or pass it in.
}

void UStageResultWidget::OnContinueClicked()
{
	// For now, just reload the current level or go to a Hub
	// FName CurrentLevel = FName(*UGameplayStatics::GetCurrentLevelName(this));
	// UGameplayStatics::OpenLevel(this, CurrentLevel);
	
	// Or close widget if it's just a summary
	// RemoveFromParent();
	
	// Typically, move to next level:
	UE_LOG(LogTemp, Log, TEXT("StageResultWidget: Continue Clicked"));
}

void UStageResultWidget::OnQuitClicked()
{
	// Return to Main Menu level
	// UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));
	UE_LOG(LogTemp, Log, TEXT("StageResultWidget: Quit Clicked"));
}

