// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/DoorBase.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"

ADoorBase::ADoorBase()
{
    PrimaryActorTick.bCanEverTick = false; // Tick is not needed for simple animation playback

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootScene);
    
    // Ensure collision is enabled so trace can hit it (BlockAll or similar in Editor)
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

    bIsOpen = false;
}

void ADoorBase::Interact_Implementation(AActor* Interactor)
{
    // Toggle state
    bIsOpen = !bIsOpen;

    UE_LOG(LogTemp, Log, TEXT("DoorBase::Interact - Door is now %s"), bIsOpen ? TEXT("Open") : TEXT("Closed"));

    UpdateDoorState();
}

void ADoorBase::UpdateDoorState()
{
    if (bIsOpen)
    {
        // Play Open Animation (Non-looping)
        if (OpenAnimation)
        {
            DoorMesh->PlayAnimation(OpenAnimation, false);

            DoorMesh->SetPlayRate(1.0f);
        }

        // Adjust Collision
        // Option A: If Physics Asset is perfect, collision moves with bones.
        // Option B (Safer): Explicitly ignore Pawn collision when open so player can pass.
        
        // Ignore the Pawn channel so the player can walk through
        DoorMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

        // Keep Visibility Blocked so we can still aim at it or trace it to close it again
        DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        
        // Keep Camera Blocked if you want camera to not clip, OR Ignore if you want checks to pass through
        DoorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); 
    }
    else
    {
        // 1. Play Close Animation (Non-looping)
        if (CloseAnimation)
        {
            DoorMesh->PlayAnimation(OpenAnimation, false);

            float AnimLength = OpenAnimation->GetPlayLength(); // Get total duration
            DoorMesh->SetPosition(AnimLength);

            DoorMesh->SetPlayRate(-1.0f);
        }

        // 2. Reset Collision to Block Player
        DoorMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        DoorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
    }
}

