// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

class UTextBlock;

/**
 * Widget for damage text fixed at a world location.
 * Requires a widget blueprint with a TextBlock named 'DamageText'.
 */
UCLASS()
class ROBOQUEST_API UDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Bind to the TextBlock in the Widget Blueprint (Must be named 'DamageText')
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	// World location where the text will be displayed
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	FVector WorldLocation;

	// Floating speed of the text (in World Units, z-axis)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float FloatSpeed = 100.0f;

	// Duration to display the text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float Duration = 1.0f;

	// Initial setup function
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayDamageText(float Damage, FVector InWorldLocation, bool bIsCritical);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

private:
	float CurrentLifeTime = 0.0f;
	FVector InitialLocation;
};
