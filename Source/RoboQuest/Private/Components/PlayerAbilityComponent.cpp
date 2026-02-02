// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerAbilityComponent.h"
#include "Abilities/RoboQuestAbility.h"
#include "RoboQuest/RoboQuestCharacter.h"

UPlayerAbilityComponent::UPlayerAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ARoboQuestCharacter>(GetOwner());
	if (!CharacterOwner) return;

	// Create ability instances
	AbilityInstanceQ = CreateAbility(AbilityClassQ);
	AbilityInstanceF = CreateAbility(AbilityClassF);
	AbilityInstancePassive = CreateAbility(AbilityClassPassive);

	// Activate passive ability immediately upon start (modify logic as needed)
	if (AbilityInstancePassive)
	{
		AbilityInstancePassive->OnPassiveActivate();
	}
}

URoboQuestAbility* UPlayerAbilityComponent::CreateAbility(TSubclassOf<URoboQuestAbility> AbilityClass)
{
	if (!AbilityClass || !CharacterOwner) return nullptr;

	// Create UObject (set Outer to this component or character)
	URoboQuestAbility* NewAbility = NewObject<URoboQuestAbility>(this, AbilityClass);
	NewAbility->InitializeAbility(CharacterOwner);
	
	return NewAbility;
}

void UPlayerAbilityComponent::PerformAbilityQ()
{
	if (AbilityInstanceQ)
	{
		AbilityInstanceQ->TryActivateAbility();
	}
}

void UPlayerAbilityComponent::PerformAbilityF()
{
	if (AbilityInstanceF)
	{
		AbilityInstanceF->TryActivateAbility();
	}
}

