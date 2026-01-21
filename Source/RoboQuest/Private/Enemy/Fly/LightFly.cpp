// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Fly/LightFly.h"

// Sets default values
ALightFly::ALightFly()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightFly::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightFly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALightFly::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

