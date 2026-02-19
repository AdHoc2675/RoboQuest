// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/SlidingDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/AmbientSound.h"
#include "Components/AudioComponent.h"

ASlidingDoor::ASlidingDoor()
{
    // Enable Tick for smooth movement interpolation
    PrimaryActorTick.bCanEverTick = true;

    // Create Root
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    // Create Door Mesh
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootScene);
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

    // Default Settings
    OpenOffset = FVector(0.0f, 0.0f, 300.0f); // Slide Up 300 units
    MoveSpeed = 5.0f;
    bIsLocked = false;
    bIsOpen = false;
}

void ASlidingDoor::BeginPlay()
{
    Super::BeginPlay();

    if (DoorMesh)
    {
        // Remember where the door was placed in the editor as the "Closed" position
        ClosedRelativeLocation = DoorMesh->GetRelativeLocation();
        TargetRelativeLocation = ClosedRelativeLocation;
    }
}

void ASlidingDoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DoorMesh)
    {
        FVector CurrentLoc = DoorMesh->GetRelativeLocation();

        // Check if we are close enough to target, if not, interpolate
        if (!CurrentLoc.Equals(TargetRelativeLocation, 0.1f))
        {
            // Smoothly move towards the target
            FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetRelativeLocation, DeltaTime, MoveSpeed);
            DoorMesh->SetRelativeLocation(NewLoc);
        }
    }
}

void ASlidingDoor::Interact_Implementation(AActor* Interactor)
{
    if (bIsLocked)
    {
        return;
    }

    // Toggle State
    SetDoorState(!bIsOpen);

    // [New Logic] Trigger Music if opening
    if (bIsOpen && bTriggersCombatMusic)
    {
        TriggerMusic();
        
        // Disable trigger so it doesn't restart music every time
        // bTriggersCombatMusic = false; 
    }
}

void ASlidingDoor::TriggerMusic()
{
    // Find all AmbientSound actors
    TArray<AActor*> AmbientSounds;
    UGameplayStatics::GetAllActorsOfClass(this, AAmbientSound::StaticClass(), AmbientSounds);

    bool bFound = false;
    for (AActor* Actor : AmbientSounds)
    {
        // Check for the specific Tag
        if (Actor->ActorHasTag(CombatMusicTag))
        {
            if (AAmbientSound* Amb = Cast<AAmbientSound>(Actor))
            {
                if (UAudioComponent* Audio = Amb->GetAudioComponent())
                {
                    // If it was silent or stopped, start/fade it in
                    if (!Audio->IsPlaying())
                    {
                        Audio->Play();
                    }
                    
                    // Fade In (Target Volume 1.0)
                    Audio->FadeIn(FadeInDuration, 1.0f);
                    bFound = true;
                    
                    UE_LOG(LogTemp, Log, TEXT("SlidingDoor::Triggered Combat Music Fade In!"));
                }
            }
        }
    }

    if (!bFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("SlidingDoor::Could not find AmbientSound with tag '%s'"), *CombatMusicTag.ToString());
    }
}

FText ASlidingDoor::GetInteractionPrompt_Implementation()
{
    if (bIsLocked)
    {
        return FText::FromString(TEXT("Locked"));
    }

    if (bIsOpen)
    {
        return FText::FromString(TEXT("[E] Close"));
    }
    else
    {
        return FText::FromString(TEXT("[E] Open"));
    }
}

void ASlidingDoor::SetLocked(bool bLockedState)
{
    bIsLocked = bLockedState;

    // If locked, usually we want to force close, but that depends on design.
    // Uncomment below if locking should auto-close.
    /*
    if (bIsLocked && bIsOpen)
    {
        SetDoorState(false);
    }
    */
}

void ASlidingDoor::SetDoorState(bool bOpenState)
{
    if (bIsOpen != bOpenState)
    {
        bIsOpen = bOpenState;

        if (bIsOpen)
        {
            // Open Action
            TargetRelativeLocation = ClosedRelativeLocation + OpenOffset;

            // Play Open Sound
            if (OpenSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
            }
        }
        else
        {
            // Close Action
            TargetRelativeLocation = ClosedRelativeLocation;

            // Play Close Sound
            if (CloseSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
            }
        }
    }
}