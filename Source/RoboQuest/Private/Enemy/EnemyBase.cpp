// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
}

bool AEnemyBase::IsAlive() const
{
	return true;
}

AActor* AEnemyBase::GetCurrentTarget() const
{
	return nullptr;
}

FVector AEnemyBase::GetAimOrigin() const
{
	return FVector();
}

void AEnemyBase::InitializeFromArchetype()
{
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


