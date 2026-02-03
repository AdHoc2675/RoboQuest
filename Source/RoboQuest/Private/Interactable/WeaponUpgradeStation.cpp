// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/WeaponUpgradeStation.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Components/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

AWeaponUpgradeStation::AWeaponUpgradeStation()
{
 	// Enable Tick to monitor hold time reset
	PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    InfoTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
    InfoTextRender->SetupAttachment(RootComponent);
    InfoTextRender->SetRelativeLocation(FVector(0, 0, 100.0f));
    InfoTextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    InfoTextRender->SetText(FText::FromString("Upgrade Weapon"));
}

void AWeaponUpgradeStation::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponUpgradeStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // If too much time passed since last Interact call, reset hold progress.
    // (Assuming Interact is called every frame while Key is held via 'Triggered' event)
    double CurrentWorldTime = GetWorld()->GetTimeSeconds();
    if (CurrentWorldTime - LastInteractTime > 0.1f)
    {
        if (CurrentHoldTime > 0.0f)
        {
            CurrentHoldTime = 0.0f;
            InfoTextRender->SetText(FText::FromString("Upgrade Canceled"));
            // Optional: Reset Visual Progress Bar here
        }
    }
}

void AWeaponUpgradeStation::Interact_Implementation(AActor* Interactor)
{
    ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(Interactor);
    if (!Player) return;

    // Get Components
    UTP_WeaponComponent* Weapon = Player->FindComponentByClass<UTP_WeaponComponent>();
    UStatusComponent* Status = Player->GetStatusComponent();

    if (!Weapon || !Status) return;

    int32 Cost = Weapon->WeaponLevel;
    int32 CurrentPower = Status->CurrentPowerCellCount;

    // Update Display Text
    FString StatusMsg = FString::Printf(TEXT("Hold E to Upgrade\nCost: %d / Power: %d"), Cost, CurrentPower);
    InfoTextRender->SetText(FText::FromString(StatusMsg));

    // Check Affordability
    if (CurrentPower < Cost)
    {
        InfoTextRender->SetText(FText::FromString("Not Enough Power!"));
        return;
    }

    // Accumulate Hold Time
    LastInteractTime = GetWorld()->GetTimeSeconds();
    CurrentHoldTime += GetWorld()->GetDeltaSeconds();

    // Visual Feedback (Simple Progress Log)
    // In real implementation, update a circular progress bar widget
    if (CurrentHoldTime >= RequiredHoldTime)
    {
        ProcessUpgrade(Player);
        CurrentHoldTime = 0.0f; // Reset after successful upgrade
    }
}

void AWeaponUpgradeStation::ProcessUpgrade(ARoboQuestCharacter* PlayerChar)
{
    if (!PlayerChar) return;

    UTP_WeaponComponent* Weapon = PlayerChar->FindComponentByClass<UTP_WeaponComponent>();
    UStatusComponent* Status = PlayerChar->GetStatusComponent();

    if (Weapon && Status)
    {
        int32 Cost = Weapon->WeaponLevel;

        // Double check cost
        if (Status->ConsumePower(Cost))
        {
            Weapon->UpgradeWeapon();
            
            // Success Feedback
            InfoTextRender->SetText(FText::FromString("UPGRADE COMPLETE!"));
            UGameplayStatics::PlaySoundAtLocation(this, Weapon->ReloadSound, GetActorLocation()); // Reuse sound or new one

            UE_LOG(LogTemp, Log, TEXT("AWeaponUpgradeStation::Upgrade Success! Level: %d"), Weapon->WeaponLevel);
        }
    }
}

