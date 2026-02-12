// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/RoboQuestGameInstance.h"
#include "RoboQuestLoadingWidget.generated.h"

class UImage;
class UTextBlock;
class UCircularThrobber; // Or just a generic widget for the spinner

/**
 * C++ Logic for the Loading Screen Widget
 */
UCLASS()
class ROBOQUEST_API URoboQuestLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Background Image component
	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;

	// Large text for Level Name (Top Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelNameText;

	// Title for the tip (Bottom Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TipTitleText;

	// Description for the tip (Bottom Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TipDescText;

	// Sets the visual data for the widget
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void UpdatePresentation(const FLevelLoadingData& Data);
};
