// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/CombatZone.h"
#include "Components/BoxComponent.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Interactable/SlidingDoor.h"

// Sets default values
ACombatZone::ACombatZone()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = false;

	bIsActive = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);

	// Default size, meant to be scaled in the level
	TriggerBox->SetBoxExtent(FVector(500.f, 500.f, 200.f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void ACombatZone::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACombatZone::OnOverlapBegin);
	}
}

void ACombatZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive && OtherActor && OtherActor->IsA(ARoboQuestCharacter::StaticClass()))
	{
		ActivateZone();
	}
}

void ACombatZone::ActivateZone()
{
	if (bIsCompleted) return;

	bIsActive = true;
	AliveEnemyCount = 0;

	// Lock all linked Sliding Doors
	for (ASlidingDoor* Door : LinkedDoors)
	{
		if (Door) Door->SetLocked(true);
	}

	// Iterate through all linked spawn points and spawn enemies
	for (AEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			AEnemyBase* SpawnedEnemy = Point->SpawnEnemy();
			if (SpawnedEnemy)
			{
				AliveEnemyCount++;
				SpawnedEnemy->OnEnemyDied.AddDynamic(this, &ACombatZone::OnEnemyKilled);
			}
		}
	}

    // Immediate complete if no enemies spawned
	if (AliveEnemyCount == 0)
	{
		CompleteZone();
	}
}

void ACombatZone::OnEnemyKilled(AEnemyBase* DeadEnemy)
{
	AliveEnemyCount--;

	if (AliveEnemyCount <= 0)
	{
		CompleteZone();
	}
}

void ACombatZone::CompleteZone()
{
	bIsActive = false;
	bIsCompleted = true;

	// Unlock all linked Sliding Doors
	for (ASlidingDoor* Door : LinkedDoors)
	{
		if (Door) Door->SetLocked(false);
	}
	// Open specific Sliding Doors if any
	for (ASlidingDoor* Door : DoorsToOpenOnComplete)
	{
		if (Door) Door->SetDoorState(true);
	}

	UE_LOG(LogTemp, Log, TEXT("Combat Zone Cleared!"));
}