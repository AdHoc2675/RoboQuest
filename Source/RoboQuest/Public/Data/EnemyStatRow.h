// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyStatRow.generated.h"

// Structure defining each row of the table
USTRUCT(BlueprintType)
struct FEnemyStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Enemy name (for Search)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyName;

	// Base maximum health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseHealth = 100.0f;

	// Base damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage = 10.0f;

	// Experience reward upon defeat
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExpReward = 50.0f;

	// Appearance (Mesh) or Animation Blueprint paths can also be managed here
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TSoftObjectPtr<USkeletalMesh> MeshAsset;
};

/**
 * 
 */
UCLASS()
class ROBOQUEST_API UEnemyStatDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
