// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickups/HealingCell.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Components/StatusComponent.h"

// Sets default values
AHealingCell::AHealingCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Sphere (Trigger)
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = SphereComponent;

	// Mesh (Physics Object)
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);
	
	// Enable Physics for the "Pop" effect on spawn
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // Don't block player movement
}

// Called when the game starts or when spawned
void AHealingCell::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap event
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealingCell::OnOverlapBegin);

	// Apply random initial impulse to simulate "dropping"
	if (MeshComponent && MeshComponent->IsSimulatingPhysics())
	{
		FVector RandomDir = FMath::VRand();
		RandomDir.Z = FMath::Abs(RandomDir.Z) + 0.5f; // Bias upwards
		MeshComponent->AddImpulse(RandomDir * SpawnImpulseStrength, NAME_None, true);
	}
}

// Called every frame
void AHealingCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			FVector TargetLoc = TargetPlayer->GetActorLocation() + FVector(0,0, 50.0f); // Aim for chest/center

			FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, TargetLoc, DeltaTime, MagnetFlySpeed);
			SetActorLocation(NewLoc);
		}
	}
}

void AHealingCell::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Only interact with Player Character
	ARoboQuestCharacter* PlayerChar = Cast<ARoboQuestCharacter>(OtherActor);
	if (PlayerChar)
	{
		// Heal Logic
		if (UStatusComponent* Status = PlayerChar->GetStatusComponent())
		{
			Status->Heal(HealAmount);
			
			// Visual FX / Sound can be added here
			// UGameplayStatics::SpawnEmitterAtLocation(...)
			// UGameplayStatics::PlaySoundAtLocation(...)
			
			Destroy();
		}
	}
}

