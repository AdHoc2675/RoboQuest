// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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
	UTextBlock* CurrentBulletText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxBulletText;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthState(float CurrentHP, float ScratchHP, float MaxHP);

};
