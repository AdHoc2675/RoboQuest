// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/AbilityDisplayWidget.h"
#include "RoboQuestAbility.generated.h"

class ARoboQuestCharacter;

/**
 * A Basic class for all abilities.
 * Capsulates logic in a lightweight UObject form without GAS.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class ROBOQUEST_API URoboQuestAbility : public UObject
{
	GENERATED_BODY()

public:
	URoboQuestAbility();

	// support getting the world from UObject
	virtual UWorld* GetWorld() const override;

	/** Initialize the ability with its owning character */
	virtual void InitializeAbility(ARoboQuestCharacter* Owner);

	/* Try to activate the ability (checks cooldown, etc.) */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool TryActivateAbility();

	/* Called when the ability is activated (implement in Blueprint or subclass) */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnActivate();

	/* For passive abilities: called immediately upon component initialization */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void OnPassiveActivate();

	/* Returns true if the ability is currently on cooldown */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsOnCooldown() const;

	/* Returns the remaining cooldown time in seconds */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetCooldownTimeRemaining() const;

public:
	/** Ability Name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	FText AbilityName;

	/** Cooldown Duration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	float CooldownDuration = 5.0f;

	/** Icon (For UI) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Info")
	UTexture2D* Icon;

	/** Ability Widget Class (For UI) */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info|UI")
	TSubclassOf<UAbilityDisplayWidget> AbilityWidgetClass;

protected:
	/** Character that owns this ability */
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
	ARoboQuestCharacter* CharacterOwner;

	/** Timer handle for managing cooldown */
	FTimerHandle CooldownTimerHandle;

	/** The last execution time */
	double LastExecutionTime = 0.0f;
};
