// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickups/PowerCell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Components/StatusComponent.h"

// Sets default values
APowerCell::APowerCell()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Sphere (Physics Object)
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // Don't trip the player
	RootComponent = MeshComponent;

	// Trigger Sphere (Detection)
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
}

void APowerCell::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap event
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APowerCell::OnOverlapBegin);

	// Apply random initial impulse to simulate "dropping" physics
	if (MeshComponent && MeshComponent->IsSimulatingPhysics())
	{
		FVector RandomDir = FMath::VRand();
		RandomDir.Z = FMath::Abs(RandomDir.Z) + 0.6f; // Bias upwards slightly more than healing cells
		MeshComponent->AddImpulse(RandomDir * SpawnImpulseStrength, NAME_None, true);
	}
}

void APowerCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsConsumed) return;

	// Find Player if not already found (optimization: cache usually done in BeginPlay, but player 0 is safe)
	if (!TargetPlayer)
	{
		TargetPlayer = UGameplayStatics::GetPlayerCharacter(this, 0);
	}

	if (TargetPlayer)
	{
		float DistSq = FVector::DistSquared(GetActorLocation(), TargetPlayer->GetActorLocation());
		float RangeSq = MagnetDetectRange * MagnetDetectRange;

		// 1. Check Magnet Condition
		if (!bIsMagnetized && DistSq <= RangeSq)
		{
			bIsMagnetized = true;
			
			// Disable physics so we can manually move it to the player
			if (MeshComponent)
			{
				MeshComponent->SetSimulatePhysics(false);
				MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Prevent getting stuck on walls while flying
			}
		}

		// 2. Execute Homing Movement
		if (bIsMagnetized)
		{
			FVector CurrentLoc = GetActorLocation();
			FVector TargetLoc = TargetPlayer->GetActorLocation() + FVector(0,0, 60.0f); // Aim for chest/center

			FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, TargetLoc, DeltaTime, MagnetFlySpeed);
			SetActorLocation(NewLoc);
		}
	}
}

void APowerCell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Prevent double triggering
	if (bIsConsumed) return;

	// Only interact with Player Character
	ARoboQuestCharacter* PlayerChar = Cast<ARoboQuestCharacter>(OtherActor);
	if (PlayerChar)
	{
		// Grant Power (Currency) Logic
		if (UStatusComponent* Status = PlayerChar->GetStatusComponent())
		{
			bIsConsumed = true;

			// Add Power Currency
			Status->AddPower(PowerAmount); 
			
			// Visual FX / Sound can be added here
			// UGameplayStatics::SpawnEmitterAtLocation(...)
			
			Destroy();
		}
	}
}

