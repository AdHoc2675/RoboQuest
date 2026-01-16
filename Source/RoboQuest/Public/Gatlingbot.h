// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Enemy/EnemyBotBase.h"
#include "Gatlingbot.generated.h"

UCLASS()
class ROBOQUEST_API AGatlingbot : public AEnemyBotBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGatlingbot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	// check if the given actor is visible
	bool CanSeeActor(AActor* TargetActor) const;

	// continuously called in Tick to make the bot look at the actor
	bool LookAtActor(AActor* TargetActor);

private:
	ARoboQuestCharacter* PlayerCharacter;

	// whether the gatling bot can see the target this frame
	bool bCanSeeTarget = false;

	// whether the gatling bot could see the target in the previous frame
	bool bPreviousCanSeeTarget = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
