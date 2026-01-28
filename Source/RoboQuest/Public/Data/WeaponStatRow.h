// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UI/CrosshairWidget.h" 
#include "WeaponStatRow.generated.h"

/** 
 * Ammo Type Enum 
 */
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Magazine UMETA(DisplayName = "Magazine"),
	Energy UMETA(DisplayName = "Energy"),
};

/** 
 * Weapon Type Enum 
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Assault UMETA(DisplayName = "Assault"),
	Precision UMETA(DisplayName = "Precision"),
	Technology UMETA(DisplayName = "Technology"),
	Demolition UMETA(DisplayName = "Demolition"), 
	CloseCombat UMETA(DisplayName = "CloseCombat")
};

/**
 * Structure mapping to DT_WeaponStat.json
 */
USTRUCT(BlueprintType)
struct FWeaponStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Basic damage per bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;

	// Number of bullets fired per shot (e.g., Shotgun = 5~10)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletCount = 1;

	// Fire rate (Rounds Per Second). e.g., 5.0 = 5 shots per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RateOfFire = 1.0f;

	// Magazine Capacity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Capacity = 30;

	// Effective range in meters
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RangeMeter = 100.0f;

	// Time required to reload
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime = 2.0f;

	// Critical damage multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CritDamage = 1.5f;

	// Ammo Type (Enum)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType = EAmmoType::Magazine;

	// Weapon Type (Enum)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::Assault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCrosshairWidget> CrosshairClass;
};

/**
 *
 */
UCLASS()
class ROBOQUEST_API UWeaponDataTable : public UDataTable
{
	GENERATED_BODY()

};
