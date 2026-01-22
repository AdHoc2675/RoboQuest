// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyPawnAIController.generated.h"

/**
 * 
 */
UCLASS()
class ROBOQUEST_API AEnemyPawnAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyPawnAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
};
