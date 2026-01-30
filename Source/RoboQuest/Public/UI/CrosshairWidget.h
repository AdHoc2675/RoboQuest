// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

class UBorder;

/**
 * 
 */
UCLASS()
class ROBOQUEST_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Sets the crosshair spread multiplier logic in C++
	UFUNCTION(BlueprintCallable, Category = "Crosshair")
	void UpdateSpread(float CurrentSpread);

protected:
	// --- Bind Widgets ---

	UPROPERTY(meta = (BindWidget))
	UBorder* Crosshair_Top;

	UPROPERTY(meta = (BindWidget))
	UBorder* Crosshair_Bottom;

	UPROPERTY(meta = (BindWidget))
	UBorder* Crosshair_Left;

	UPROPERTY(meta = (BindWidget))
	UBorder* Crosshair_Right;

	// --- Config ---

	/** Multiplier to convert spread angle to pixel offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	float SpreadScale = 50.0f;
};
