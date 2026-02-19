// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageResultWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UTexture2D;
class UAudioComponent;

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

	// Optional: Keep text for fallback or debug
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RankText; 

	// Bind to an Image widget named 'RankImage' in the Designer
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* RankImage;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// --- Assets (Assign these in the Widget Blueprint Defaults) ---

	UPROPERTY(EditDefaultsOnly, Category = "Stage Result|Assets")
	UTexture2D* RankTextureS;

	UPROPERTY(EditDefaultsOnly, Category = "Stage Result|Assets")
	UTexture2D* RankTextureA;

	UPROPERTY(EditDefaultsOnly, Category = "Stage Result|Assets")
	UTexture2D* RankTextureB;
	
	// Fallback for lower ranks if needed
	UPROPERTY(EditDefaultsOnly, Category = "Stage Result|Assets")
	UTexture2D* RankTextureDefault;

	// --- Animations ---
	
	// Fade-in animation for the widget content
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* IntroAnim;

	// --- Audio Settings ---

	// Background music to play when this screen is shown
	UPROPERTY(EditDefaultsOnly, Category = "Stage Result|Audio")
	USoundBase* ResultBGM;

	// --- Internal State ---

	UPROPERTY()
	UAudioComponent* BGMComponent;

	// --- Event Handlers ---

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnQuitClicked();
};
