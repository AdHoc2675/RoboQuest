// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/CombatZone.h"
#include "Components/BoxComponent.h"
#include "RoboQuest/RoboQuestCharacter.h"

// Sets default values
ACombatZone::ACombatZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	// Check if triggered by player and not already active
	if (!bIsActive && OtherActor && OtherActor->IsA(ARoboQuestCharacter::StaticClass()))
	{
		ActivateZone();
	}
}

void ACombatZone::ActivateZone()
{
	bIsActive = true;

	// Iterate through all linked spawn points and spawn enemies
	for (AEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();

			// Optional: Destroy the spawn point actor to clean up memory,
			// since it's just a marker.
			// Point->Destroy();
		}
	}

	// Additional Logic:
	// - Lock doors
	// - Start background music
	// - Notify GameMode
}

