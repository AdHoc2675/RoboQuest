// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/CrosshairWidget.h"
#include "BaseUserHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROBOQUEST_API UBaseUserHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthScratchBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EXPBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentBulletText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxBulletText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeedText;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Top;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Bottom;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Left;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Right;

	/** Multiplier to convert spread angle to pixel offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Crosshair")
	float SpreadScale = 30.0f;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthState(float CurrentHP, float ScratchHP, float MaxHP);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateAmmoState(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateExpState(float CurrentExp, float MaxExp, int32 CurrentLevel);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerStats(float Shield, float Speed);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateCrosshairSpread(float Spread);
};
