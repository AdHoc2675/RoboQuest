// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/RoboQuestGameInstance.h"
#include "RoboQuestLoadingWidget.generated.h"

class UImage;
class UTextBlock;
class UWidget;
class USoundBase;

/**
 * C++ Logic for the Loading Screen Widget
 */
UCLASS()
class ROBOQUEST_API URoboQuestLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Image
	// -------------------------------------------------------------

	// The furthest background (Sky/Mountains/Buildings) - Moves slowly
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* FarBackgroundImage;

	// The main background (Landscape) - Moves faster
	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;

	// The Bus/Character - Bobs up and down
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* BusImage;

	// -------------------------------------------------------------

	// Large text for Level Name (Top Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelNameText;

	// Title for the tip (Bottom Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TipTitleText;

	// Description for the tip (Bottom Left)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TipDescText;

	// Panel showing "Loading..." text and throbber (Visible during loading)
	UPROPERTY(meta = (BindWidget))
	UWidget* LoadingIndicatorPanel;

	// Panel showing "Press any key to continue" (Visible when finished)
	UPROPERTY(meta = (BindWidget))
	UWidget* PressKeyPanel;

	// Background Music to loop during loading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* LoadingBGM;

	// Sound effect to play when loading finishes (Ready state)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* LoadingCompleteSound;

	// Sets the visual data and starts BGM
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void UpdatePresentation(const FLevelLoadingData& Data);

	// Called by GameInstance when the fake loading timer finishes
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void SetLoadingComplete();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Handle input to proceed
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	// Tracks if loading is finished and waiting for input
	bool bIsLoadingComplete = false;

	// New flag to prevent multiple triggers
	bool bHasProceeded = false;

	// Function to trigger level transition
	void ProceedToLevel();

	float TotalTime = 0.0f;
};
