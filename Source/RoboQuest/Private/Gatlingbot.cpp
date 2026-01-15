// Fill out your copyright notice in the Description page of Project Settings.


#include "Gatlingbot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGatlingbot::AGatlingbot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create SkeletalMeshComponent and attach to root
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GatlingMesh"));
	SetRootComponent(SkeletalMeshComponent);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/RoboQuestAsset/GatlingBot/RoboTestAsset/GatlingBot/TurretTest2.TurretTest2"));

	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(MeshAsset.Object);
	}

	// Set relative location and rotation
	SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, -90.0f, 0.0f));

	// Animation Blueprint
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/BP/ABP_Gatlingbot.ABP_Gatlingbot_C"));
	if (AnimBP.Succeeded())
	{
		SkeletalMeshComponent->SetAnimInstanceClass(AnimBP.Class);
	}
}

bool AGatlingbot::CanSeeActor(AActor* TargetActor) const
{
	if (TargetActor == nullptr)
	{
		return false;
	}

	// save line trace result
	FHitResult Hit;

	// set start and end location
	FVector Start = GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	// only operate on Visible channel
	ECollisionChannel Channel = ECC_Visibility;

	// parameters to add to line trace
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this); // ignore self

	// debug line
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	// execute line trace

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel, CollisionQueryParams);

	return !Hit.IsValidBlockingHit();
}

bool AGatlingbot::LookAtActor(AActor* TargetActor)
{
	if (TargetActor == nullptr) return false;

	if (CanSeeActor(TargetActor))
	{
		FVector Start = GetActorLocation();
		FVector End = TargetActor->GetActorLocation();

		// get rotation value to look at target
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End) + FRotator(0.0f, -90.0f, 0.0f);
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;

		// rotate to face the target
		SetActorRotation(LookAtRotation);

		return true;
	}

	return false;
}

// Called when the game starts or when spawned
void AGatlingbot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGatlingbot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Find the player character in the world
	ACharacter* CurrentPlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	bCanSeeTarget = LookAtActor(CurrentPlayerCharacter);



	bPreviousCanSeeTarget = bCanSeeTarget;
}

// Called to bind functionality to input
void AGatlingbot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

