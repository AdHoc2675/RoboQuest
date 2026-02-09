// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/RoboQuestAbility.h"
#include "Ability_Frenzy.generated.h"

/**
 * Passive Ability: Frenzy
 * - Generates stacks on Takedowns (+2) and Boss Hits by Abilities (+4).
 * - Increases Firerate(1%), Reload Speed(1%), Ability Cooldown Speed(1%), Movement Speed(0.5%) per stack.
 * - Decays over time (1.25 stacks/sec).
 */
UCLASS()
class ROBOQUEST_API UAbility_Frenzy : public URoboQuestAbility
{
	GENERATED_BODY()

public:
	UAbility_Frenzy();

	virtual void InitializeAbility(ARoboQuestCharacter* Owner) override;

    // Hook events here
	virtual void OnPassiveActivate_Implementation() override;

protected:
	// Max stacks limit (Base 16)
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float MaxStacks = 16.0f;

	// Decay rate per second
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float DecayRate = 1.25f;

	// Stacks generated on Takedown
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float StacksPerKill = 2.0f;

	// Stacks generated on Boss Hit by Ability
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Config")
	float StacksPerBossHit = 4.0f;

private:
	// Current Frenzy Stacks
	float CurrentStacks = 0.0f;

	// Timer for handling decay/update loop
	FTimerHandle UpdateTimerHandle;

	// Update interval (e.g. 0.1s)
	float UpdateInterval = 0.1f;

	// Functions bound to delegates
	UFUNCTION()
	void HandleEnemyKilled();

	UFUNCTION()
	void HandleBossHit();

	// Internal logic
	void AddStacks(float Amount);
	void UpdateFrenzyLoop();
	void ApplyBuffs();
};
