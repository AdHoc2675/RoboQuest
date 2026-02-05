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

    if (BenchMesh)
    {
        UMaterialInterface* BaseMat = BenchMesh->GetMaterial(EmissionMaterialIndex);
        if (BaseMat)
        {
            DynamicMaterial = BenchMesh->CreateDynamicMaterialInstance(EmissionMaterialIndex, BaseMat);
            
            if (DynamicMaterial)
            {
                // Initialize Black/Off
                DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, FLinearColor::Black);
                DynamicMaterial->SetVectorParameterValue(BaseColorParamName, FLinearColor::Black);
            }
        }
    }
}

// Called every frame
void AWeaponRarityUpgradeBench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    
    if (LastInteractTime > 0.0 && LastInteractorCharacter)
    {
        UpdateBenchColor(LastInteractorCharacter);
    }
    else
    {
        // Reset color to default when idle (Optional)
        if (DynamicMaterial)
        {
             // Slowly fade out or reset? For now, just keep last or reset to Idle Color
             // DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, FLinearColor::Black);
        }
    }

	// Clear HUD text after 3 seconds of showing status result
	if (LastInteractTime > 0.0 && (GetWorld()->GetTimeSeconds() - LastInteractTime > 3.0f))
	{
        if (DynamicMaterial)
        {
            // Reset color when interaction 'session' ends
            DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, FLinearColor::Black); 
        }

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

    // Update Color immediately on interaction attempt
    UpdateBenchColor(Player);

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

        // Update Color for the NEW rarity immediately
        UpdateBenchColor(Player);

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

void AWeaponRarityUpgradeBench::UpdateBenchColor(ARoboQuestCharacter* Player)
{
    if (!Player || !DynamicMaterial) return;
    
    UTP_WeaponComponent* WeaponComp = Player->GetCurrentWeapon();
    if (WeaponComp)
    {
        // Get the deep, rich color
        FLinearColor RarityColor = GetRarityColor(WeaponComp->WeaponRarity);
        
        // 1. Set Base Color (The object's actual surface color)
        // This ensures it looks colored even without the glow washing it out
        DynamicMaterial->SetVectorParameterValue(BaseColorParamName, RarityColor);

        // 2. Set Emissive (Glow)
        // Multiply by Intensity. Too high intensity turns colors white (Bloom).
        DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, RarityColor * EmissionIntensity);
    }
}

FLinearColor AWeaponRarityUpgradeBench::GetRarityColor(EWeaponRarity Rarity) const
{
    // Return deeper, more saturated colors for better visuals
    switch (Rarity)
    {
        case EWeaponRarity::Common:    return FLinearColor(0.2f, 0.2f, 0.2f); // Dark Grey (Not pure white)
        case EWeaponRarity::Uncommon:  return FLinearColor(0.0f, 1.0f, 0.05f); // Pure Green
        case EWeaponRarity::Rare:      return FLinearColor(0.0f, 0.2f, 1.0f); // Deep Blue
        case EWeaponRarity::Epic:      return FLinearColor(0.4f, 0.0f, 1.0f); // Deep Purple
        case EWeaponRarity::Fantastic: return FLinearColor(1.0f, 0.15f, 0.0f); // Deep Red-Orange
        default: return FLinearColor::White;
    }
}

