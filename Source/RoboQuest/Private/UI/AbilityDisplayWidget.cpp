// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/AbilityDisplayWidget.h"
#include "Abilities/RoboQuestAbility.h"

void UAbilityDisplayWidget::BindAbility(URoboQuestAbility* NewAbility)
{
	BoundAbility = NewAbility;
	OnAbilityBound();
}