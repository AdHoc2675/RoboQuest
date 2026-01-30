// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/RoboQuestAbility.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "TimerManager.h"

URoboQuestAbility::URoboQuestAbility()
{
}

UWorld* URoboQuestAbility::GetWorld() const
{
	if (IsTemplate() || !GetOuter()) return nullptr;
	return GetOuter()->GetWorld();
}

void URoboQuestAbility::InitializeAbility(ARoboQuestCharacter* Owner)
{
	CharacterOwner = Owner;
}

bool URoboQuestAbility::TryActivateAbility()
{
	if (!CharacterOwner || IsOnCooldown())
	{
		return false;
	}

	// Start cooldown
	if (UWorld* World = GetWorld())
	{
		LastExecutionTime = World->GetTimeSeconds();
		World->GetTimerManager().SetTimer(CooldownTimerHandle, CooldownDuration, false);
	}

	OnActivate();
	return true;
}

void URoboQuestAbility::OnActivate_Implementation()
{
	// Override in child class (C++ or BP)
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Ability %s Activated!"), *AbilityName.ToString()));
}

void URoboQuestAbility::OnPassiveActivate_Implementation()
{
	// Passive logic
}

bool URoboQuestAbility::IsOnCooldown() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().IsTimerActive(CooldownTimerHandle);
	}
	return false;
}

float URoboQuestAbility::GetCooldownTimeRemaining() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
	}
	return 0.0f;
}