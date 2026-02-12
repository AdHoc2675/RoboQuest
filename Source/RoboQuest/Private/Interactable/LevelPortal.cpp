// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/LevelPortal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "System/RoboQuestGameInstance.h"
#include "Kismet/GameplayStatics.h"

ALevelPortal::ALevelPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	RootComponent = PortalMesh;

	InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(RootComponent);
	InteractionZone->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	InteractionZone->SetCollisionProfileName(TEXT("Trigger"));
}

void ALevelPortal::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelPortal::Interact_Implementation(AActor* Interactor)
{
	if (LevelToLoad.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelPortal::No Level Name specified!"));
		return;
	}

	// Get GameInstance and call the loading function
	if (URoboQuestGameInstance* GI = Cast<URoboQuestGameInstance>(GetGameInstance()))
	{
		UE_LOG(LogTemp, Log, TEXT("LevelPortal::Transitioning to %s..."), *LevelToLoad.ToString());
		GI->OpenLevelWithFakeLoading(LevelToLoad, LoadingDuration);
	}
	else
	{
		// Fallbox if GameInstance is not set up correctly
		UGameplayStatics::OpenLevel(this, LevelToLoad);
	}
}

FText ALevelPortal::GetInteractionPrompt_Implementation()
{
	FString LevelNameStr = LevelToLoad.ToString();
	return FText::FromString(FString::Printf(TEXT("[E] to Next Level")));
}