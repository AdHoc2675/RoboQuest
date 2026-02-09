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

	// Duration to display the text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float Duration = 1.0f;

	// --- Physics & Animation Settings ---
	
	// Initial upward burst speed (Base Value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float InitialVerticalSpeed = 350.0f;

	// Horizontal scatter speed (Base Value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float HorizontalSpeed = 120.0f;

	// Downward gravity acceleration (Base Value)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float Gravity = 490.0f;

	// --- Distance Scaling (New) ---

	// The reference distance where Scale is 1.0. (e.g. 10m = 1000 units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float ReferenceDistance = 1000.0f;

	// Minimum scale for very close targets (Prevents text from flying off screen)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float MinScale = 0.5f;

	// Maximum scale for very far targets (Prevents text from moving too wildly)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
	float MaxScale = 2.5f;


	// Initial setup function
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayDamageText(float Damage, FVector InWorldLocation, bool bIsCritical, FLinearColor TextColor = FLinearColor::White);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

private:
	float CurrentLifeTime = 0.0f;
	
	// For physics simulation
	FVector Velocity;
	float AppliedGravity; // Scaled gravity
	
	// For scaling animation
	FVector2D StartScale;
	FVector2D TargetScale;
};
