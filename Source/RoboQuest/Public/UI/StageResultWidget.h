// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageResultWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;

/**
 * UStageResultWidget: Handles the display of stage completion statistics.
 * Design the visual layout in a Blueprint inheriting from this class.
 */
UCLASS()
class ROBOQUEST_API UStageResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// --- Public API ---
	
	// Setup the data to display (called by the door or game instance)
	UFUNCTION(BlueprintCallable, Category = "Stage Result")
	void SetupResultData(FName StageName, float PlayTime, int32 DefeatedEnemies, FString Rank);

protected:
	// --- Bindings (Create these in Widget Designer) ---

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* StageNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayTimeText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RankText;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// --- Animations ---
	
	// Fade-in animation for the widget content
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* IntroAnim;

	// --- Event Handlers ---

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnQuitClicked();
};
