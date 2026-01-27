// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/DoorBase.h"
#include "Components/SceneComponent.h"

ADoorBase::ADoorBase()
{
    PrimaryActorTick.bCanEverTick = true; // Use true if you want to update sliding logic in Tick

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootScene);

    bIsOpen = false;
}

void ADoorBase::Interact_Implementation(AActor* Interactor)
{
    // Toggle state
    bIsOpen = !bIsOpen;

    if (bIsOpen)
    {
        // Logic to play open animation
        // Since C++ handling of raw skeletal animations on static meshes is complex, 
        // we heavily rely on Blueprints to drive the Timeline or PlayAnimation.
        PlayOpenEffects();
    }
    else
    {
        PlayCloseEffects();
    }
}

