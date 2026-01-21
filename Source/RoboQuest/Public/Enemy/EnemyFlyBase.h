// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "EnemyFlyBase.generated.h"

/**
 * AEnemyFlyBase is Flying enemy base class
 */
UCLASS()
class ROBOQUEST_API AEnemyFlyBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyFlyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
