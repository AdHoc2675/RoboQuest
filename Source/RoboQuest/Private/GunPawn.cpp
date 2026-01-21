// Fill out your copyright notice in the Description page of Project Settings.


#include "GunPawn.h"
#include "Components/StatusComponent.h"

// Sets default values
AGunPawn::AGunPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    // Find the Skeletal Mesh Asset
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/RoboQuestAsset/GunPawn/GunPawn.GunPawn"));

	if (MeshAsset.Succeeded())
	{
		// Use the inherited Mesh component from ACharacter
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	// Set relative location and rotation of the inherited Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f)); // Adjusted Z to -90 to align with Capsule typically
}

// Called when the game starts or when spawned
void AGunPawn::BeginPlay()
{
	Super::BeginPlay();
	
    // StatusComponent is initialized in AEnemyBase constructor
	if (StatusComponent)
	{
		StatusComponent->InitializeEnemyStats("GunPawn", 1);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AGunPawn::BeginPlay() - StatusComponent is NULL!"));
	}
}

// Called every frame
void AGunPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGunPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

