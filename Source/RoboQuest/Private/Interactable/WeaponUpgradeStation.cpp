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
	PrimaryActorTick.bCanEverTick = true; // Still use tick to reset text after a few seconds

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    InfoTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
    InfoTextRender->SetupAttachment(RootComponent);
    InfoTextRender->SetRelativeLocation(FVector(0, 0, 100.0f));
    InfoTextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    InfoTextRender->SetText(FText::FromString("Interact (E)\nto Upgrade"));
}

void AWeaponUpgradeStation::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponUpgradeStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Simple Logic: Reset text to default after 3 seconds of inactivity
    if (GetWorld()->GetTimeSeconds() - LastInteractTimer > 3.0f)
    {
        UpdateInfoText(TEXT("Interact (E)\nto Upgrade"));
    }
}

// 1. One-Shot Interaction (Started) - Shows Status
void AWeaponUpgradeStation::Interact_Implementation(AActor* Interactor)
{
    ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(Interactor);
    if (!Player) return;

    LastInteractTimer = GetWorld()->GetTimeSeconds();

    UTP_WeaponComponent* Weapon = Player->FindComponentByClass<UTP_WeaponComponent>();
    UStatusComponent* Status = Player->GetStatusComponent();

    if (Weapon && Status)
    {
        // Cost equals current weapon level. (Lv1 -> Cost 1)
        int32 Cost = Weapon->WeaponLevel;
        int32 CurrentPower = Status->CurrentPowerCellCount;

        // Try Upgrade
        if (Status->ConsumePower(Cost))
        {
            Weapon->UpgradeWeapon();
            UpdateInfoText(FString::Printf(TEXT("SUCCESS!\nLv %d -> %d"), Cost, Weapon->WeaponLevel));
            
			UE_LOG(LogTemp, Log, TEXT("WeaponUpgradeStation::Interact - Upgraded weapon to Level %d for %d Power"), Weapon->WeaponLevel, Cost);

            if (Weapon->ReloadSound)
            {
                 UGameplayStatics::PlaySoundAtLocation(this, Weapon->ReloadSound, GetActorLocation());
            }
        }
        else
        {
            UpdateInfoText(FString::Printf(TEXT("Need %d Power!\nCurrent: %d"), Cost, CurrentPower));
        }
    }
}

void AWeaponUpgradeStation::ProcessUpgrade(ARoboQuestCharacter* PlayerChar)
{
    // Logic moved to Interact_Implementation for simplicity
}

void AWeaponUpgradeStation::UpdateInfoText(FString NewText)
{
    if (InfoTextRender)
    {
        InfoTextRender->SetText(FText::FromString(NewText));
    }
}

