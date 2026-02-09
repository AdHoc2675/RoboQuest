// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/CrosshairWidget.h"
#include "Components/NamedSlot.h"
#include "BaseUserHUDWidget.generated.h"

class URoboQuestAbility;
class UWeaponDetailWidget;

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

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PowerCellCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractionMsgText;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Top;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Bottom;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Left;

	UPROPERTY(meta = (BindWidgetOptional))
	UBorder* Crosshair_Right;

	UPROPERTY(meta = (BindWidget))
	UNamedSlot* Slot_AbilityQ;

	UPROPERTY(meta = (BindWidget))
	UNamedSlot* Slot_AbilityF;

	UPROPERTY(meta = (BindWidget))
	UNamedSlot* Slot_AbilityPassive;

	// Slot specifically for the Weapon Card UI
	UPROPERTY(meta = (BindWidget))
	UNamedSlot* Slot_WeaponInfo;

	// Class of the Weapon Detail Widget to spawn
	UPROPERTY(EditDefaultsOnly, Category = "UI|Weapon")
	TSubclassOf<UWeaponDetailWidget> WeaponDetailWidgetClass;

	// Cached reference to the spawned widget
	UPROPERTY(BlueprintReadOnly, Category = "UI|Weapon")
	UWeaponDetailWidget* ActiveWeaponDetailWidget;


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
	void UpdatePowerState(int32 CurrentPowerCellCount);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInteractionMessage(FString Message, FLinearColor Color = FLinearColor::White);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateCrosshairSpread(float Spread);

	void AssignAbilityToSlot(UNamedSlot* TargetSlot, URoboQuestAbility* Ability);

	// Initialize the weapon slot with the specific widget class
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializeWeaponSlot(class UTP_WeaponComponent* WeaponComp);

	// Toggle visibility of the weapon detail card
	UFUNCTION(BlueprintCallable, Category = "UI|Weapon")
	void SetWeaponDetailVisibility(bool bVisible);
};
