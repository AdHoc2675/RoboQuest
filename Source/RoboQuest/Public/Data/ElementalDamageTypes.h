// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ElementalDamageTypes.generated.h"

/**
 * Burn Damage Type (Orange/Red)
 */
UCLASS()
class ROBOQUEST_API UDamageTypeBurn : public UDamageType
{
	GENERATED_BODY()
};

/**
 * Cryo Damage Type (Sky Blue)
 */
UCLASS()
class ROBOQUEST_API UDamageTypeCryo : public UDamageType
{
	GENERATED_BODY()
};

/**
 * Shock Damage Type (Purple)
 */
UCLASS()
class ROBOQUEST_API UDamageTypeShock : public UDamageType
{
	GENERATED_BODY()
};
