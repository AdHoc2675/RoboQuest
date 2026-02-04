// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/WeaponUpgradeBench.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Components/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponUpgradeBench::AWeaponUpgradeBench()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	BenchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BenchMesh"));
	RootComponent = BenchMesh;
    BenchMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

// Called when the game starts or when spawned
void AWeaponUpgradeBench::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeaponUpgradeBench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Clear HUD text after 3 seconds
	if (LastInteractTime > 0.0 && (GetWorld()->GetTimeSeconds() - LastInteractTime > 3.0f))
	{
        if (LastInteractorCharacter)
        {
            // Clear the message
            UpdateHUDMessage(LastInteractorCharacter, TEXT(""), FLinearColor::White);
            LastInteractorCharacter = nullptr;
        }
		LastInteractTime = -1.0; 
	}
}

void AWeaponUpgradeBench::Interact_Implementation(AActor* Interactor)
{
    ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(Interactor);
    if (!Player) return;

	LastInteractTime = GetWorld()->GetTimeSeconds();
    LastInteractorCharacter = Player;

    // 1. Get Components
    UTP_WeaponComponent* WeaponComp = Player->GetCurrentWeapon();
    
    // Fallback search if current weapon is null (safeguard)
    if (!WeaponComp)
    {
         WeaponComp = Player->FindComponentByClass<UTP_WeaponComponent>();
    }

    UStatusComponent* StatusComp = Player->GetStatusComponent();

    if (!WeaponComp)
    {
        UpdateHUDMessage(Player, TEXT("No Weapon Equipped"), FLinearColor::Red);
        if (UpgradeFailSound) UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
        return;
    }

    if (!StatusComp) return;

    // 2. Calculate Cost
    int32 UpgradeCost = WeaponComp->WeaponLevel;
    int32 CurrentPower = StatusComp->CurrentPowerCellCount;

    // 3. Attempt Purchase
    if (StatusComp->ConsumePower(UpgradeCost))
    {
        // Success
        int32 OldLevel = WeaponComp->WeaponLevel;
        WeaponComp->UpgradeWeapon();
        int32 NewLevel = WeaponComp->WeaponLevel;

        FString SuccessMsg = FString::Printf(TEXT("UPGRADED!\nLv.%d -> Lv.%d (-%d Power)"), OldLevel, NewLevel, UpgradeCost);
        UpdateHUDMessage(Player, SuccessMsg, FLinearColor::Green);

		UE_LOG(LogTemp, Log, TEXT("WeaponUpgradeBench::Weapon upgraded from Level %d to Level %d"), OldLevel, NewLevel);
        
        if (UpgradeSuccessSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, UpgradeSuccessSound, GetActorLocation());
        }
        else if (WeaponComp->ReloadSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, WeaponComp->ReloadSound, GetActorLocation());
        }
    }
    else
    {
        // Not enough currency
        FString FailMsg = FString::Printf(TEXT("Insuffient Power!\nCost: %d / Have: %d"), UpgradeCost, CurrentPower);
        UpdateHUDMessage(Player, FailMsg, FLinearColor::Red);

        if (UpgradeFailSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
        }
    }
}

FText AWeaponUpgradeBench::GetInteractionPrompt_Implementation()
{
    return FText::FromString(TEXT("[E] to Upgrade Weapon"));
}

void AWeaponUpgradeBench::UpdateHUDMessage(ARoboQuestCharacter* Player, FString NewText, FLinearColor Color)
{
    if (Player)
    {
        if (UBaseUserHUDWidget* HUD = Player->GetHUDWidget())
        {
            HUD->SetInteractionMessage(NewText, Color);
        }
    }
}

