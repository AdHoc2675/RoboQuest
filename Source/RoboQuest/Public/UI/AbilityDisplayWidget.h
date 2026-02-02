// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityDisplayWidget.generated.h"

class URoboQuestAbility;
class UImage;
class UTextBlock;

/**
 * Widget that displays an ability icon and its cooldown status using a numeric overlay
 * and grayscale tinting logic handled in C++.
 */
UCLASS()
class ROBOQUEST_API UAbilityDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* Binds an ability instance to this widget to track its state */
	UFUNCTION(BlueprintCallable, Category = "Ability UI")
	void BindAbility(URoboQuestAbility* NewAbility);

	/* Called via Blueprint event after binding is complete (for extra BP logic) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability UI")
	void OnAbilityBound();

protected:
	virtual void NativeConstruct() override;

	/* Override the NativeTick to update UI elements every frame */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// --- Bindable Widgets (Must match variables in Widget Blueprint) ---

	/* The icon image of the ability */
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* AbilityIcon;

	/* The text block displaying remaining cooldown time */
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CooldownText;

	/* The ability currently bound to this widget (Weak Pointer for safety) */
	TWeakObjectPtr<URoboQuestAbility> BoundAbility;
};
