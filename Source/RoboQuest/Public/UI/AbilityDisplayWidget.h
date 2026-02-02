// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityDisplayWidget.generated.h"

class URoboQuestAbility;

/**
 * 
 */
UCLASS()
class ROBOQUEST_API UAbilityDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* Binds an ability to this widget */
	UFUNCTION(BlueprintCallable, Category = "Ability UI")
	void BindAbility(URoboQuestAbility* NewAbility);

	/* called when the ability is bound to the widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability UI")
	void OnAbilityBound();

protected:
	/* The ability bound to this widget */
	UPROPERTY(BlueprintReadOnly, Category = "Ability UI")
	URoboQuestAbility* BoundAbility;
};
