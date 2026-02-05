// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/WeaponRarityUpgradeBench.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Components/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/BaseUserHUDWidget.h"

// Sets default values
AWeaponRarityUpgradeBench::AWeaponRarityUpgradeBench()
{
	PrimaryActorTick.bCanEverTick = true;

	BenchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BenchMesh"));
	RootComponent = BenchMesh;
	BenchMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

// Called when the game starts or when spawned
void AWeaponRarityUpgradeBench::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeaponRarityUpgradeBench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Clear HUD text after 3 seconds of showing status result
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

int32 AWeaponRarityUpgradeBench::GetUpgradeCost(EWeaponRarity CurrentRarity) const
{
	// Cost Sequence: 4 -> 6 -> 8 -> 10
	switch (CurrentRarity)
	{
	case EWeaponRarity::Common:    return 4;
	case EWeaponRarity::Uncommon:  return 6;
	case EWeaponRarity::Rare:      return 8;
	case EWeaponRarity::Epic:      return 10;
	case EWeaponRarity::Fantastic: return -1; // Max Level
	default: return 999;
	}
}

void AWeaponRarityUpgradeBench::Interact_Implementation(AActor* Interactor)
{
	ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(Interactor);
	if (!Player) return;

	LastInteractTime = GetWorld()->GetTimeSeconds();
	LastInteractorCharacter = Player;

	// 1. Get Components
	UTP_WeaponComponent* WeaponComp = Player->GetCurrentWeapon();
	
	// Check if weapon exists
	if (!WeaponComp)
	{
		UpdateHUDMessage(Player, TEXT("No Weapon Equipped"), FLinearColor::Red);
		if (UpgradeFailSound) UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
		return;
	}

	UStatusComponent* StatusComp = Player->GetStatusComponent();
	if (!StatusComp) return;

	// 2. Calculate Cost & Rarity
	EWeaponRarity CurrentRarity = WeaponComp->WeaponRarity;
	int32 Cost = GetUpgradeCost(CurrentRarity);

	// Check if Maxed
	if (Cost < 0)
	{
		UpdateHUDMessage(Player, TEXT("Max Rarity Reached!"), FLinearColor::Yellow);
		return;
	}

	// 3. Attempt Purchase
	if (StatusComp->ConsumePower(Cost))
	{
		// Upgrade Logic: Increment Enum
		EWeaponRarity NextRarity = static_cast<EWeaponRarity>((uint8)CurrentRarity + 1);
		WeaponComp->WeaponRarity = NextRarity;

		// Success Message
		FString SuccessMsg = FString::Printf(TEXT("RARITY UPGRADED!\n-%d Power"), Cost);
		UpdateHUDMessage(Player, SuccessMsg, FLinearColor::Green);

		UE_LOG(LogTemp, Log, TEXT("WeaponRarityUpgradeBench::Upgraded rarity to %d"), (uint8)NextRarity);

		// Play Sound
		if (UpgradeSuccessSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UpgradeSuccessSound, GetActorLocation());
		}

		// Force refresh the Weapon Detail UI
		if (UBaseUserHUDWidget* HUD = Player->GetHUDWidget())
		{
			HUD->InitializeWeaponSlot(WeaponComp);
		}
	}
	else
	{
		// Insufficient Funds
		FString FailMsg = FString::Printf(TEXT("Need %d Power!\nCurrent: %d"), Cost, StatusComp->CurrentPowerCellCount);
		UpdateHUDMessage(Player, FailMsg, FLinearColor::Red);

		if (UpgradeFailSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
		}
	}
}

FText AWeaponRarityUpgradeBench::GetInteractionPrompt_Implementation()
{
	// We can't access the specific weapon instance easily here without casting logic similar to interact
	// Simple prompt for now. Ideally, dynamic prompts would check the player's weapon.
	return FText::FromString(TEXT("[E] Upgrade Rarity"));
}

bool AWeaponRarityUpgradeBench::ShouldShowWeaponDetail_Implementation()
{
	// Trigger the weapon detail card on HUD
	return true;
}

void AWeaponRarityUpgradeBench::UpdateHUDMessage(ARoboQuestCharacter* Player, FString NewText, FLinearColor Color)
{
	if (Player)
	{
		if (UBaseUserHUDWidget* HUD = Player->GetHUDWidget())
		{
			HUD->SetInteractionMessage(NewText, Color);
		}
	}
}

