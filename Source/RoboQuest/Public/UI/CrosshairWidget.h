// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROBOQUEST_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Sets the crosshair spread multiplier
	UFUNCTION(BlueprintImplementableEvent, Category = "Crosshair")
	void UpdateSpread(float CurrentSpread);
};
