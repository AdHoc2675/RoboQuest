// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAbilityComponent.generated.h"

class URoboQuestAbility;
class ARoboQuestCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOQUEST_API UPlayerAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerAbilityComponent();

protected:
	virtual void BeginPlay() override;

public:	
	/** Q Key Ability Class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<URoboQuestAbility> AbilityClassQ;

	/** F Key Ability Class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<URoboQuestAbility> AbilityClassF;

	/** Passive Ability Class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<URoboQuestAbility> AbilityClassPassive;

	// --- Getters for created instances ---
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	URoboQuestAbility* GetAbilityQ() const { return AbilityInstanceQ; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	URoboQuestAbility* GetAbilityF() const { return AbilityInstanceF; }

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	URoboQuestAbility* GetAbilityPassive() const { return AbilityInstancePassive; }

	// --- Input handling functions ---

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void PerformAbilityQ();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void PerformAbilityF();

private:
	/** Instances of the abilities created from the specified classes */
	UPROPERTY()
	URoboQuestAbility* AbilityInstanceQ;

	UPROPERTY()
	URoboQuestAbility* AbilityInstanceF;

	UPROPERTY()
	URoboQuestAbility* AbilityInstancePassive;

	ARoboQuestCharacter* CharacterOwner;

	/** Internal helper: Create ability instance */
	URoboQuestAbility* CreateAbility(TSubclassOf<URoboQuestAbility> AbilityClass);
};
