// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/WeaponUpgradeBench.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "Components/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponUpgradeBench::AWeaponUpgradeBench()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BenchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BenchMesh"));
	RootComponent = BenchMesh;
    // Set a default mesh or shape if needed via Blueprint, or use a placeholder cube in editor.
    BenchMesh->SetCollisionProfileName(TEXT("BlockAll"));

	InfoTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("InfoTextRender"));
	InfoTextRender->SetupAttachment(RootComponent);
	InfoTextRender->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f)); // Position above the bench
	InfoTextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	InfoTextRender->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	InfoTextRender->SetWorldSize(20.0f);
	InfoTextRender->SetTextRenderColor(FColor::Cyan);
	InfoTextRender->SetText(FText::FromString(DefaultMessage));
}

// Called when the game starts or when spawned
void AWeaponUpgradeBench::BeginPlay()
{
	Super::BeginPlay();
	SetInfoText(DefaultMessage, FColor::Cyan);
}

// Called every frame
void AWeaponUpgradeBench::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reset text to default after 3 seconds of showing status result
	if (LastInteractTime > 0.0 && (GetWorld()->GetTimeSeconds() - LastInteractTime > 3.0f))
	{
		SetInfoText(DefaultMessage, FColor::Cyan);
		LastInteractTime = -1.0; // Stop checking until next interact
	}
}

void AWeaponUpgradeBench::Interact_Implementation(AActor* Interactor)
{
    ARoboQuestCharacter* Player = Cast<ARoboQuestCharacter>(Interactor);
    if (!Player) 
    {
        return; 
    }

	LastInteractTime = GetWorld()->GetTimeSeconds();

    // Get Components
    // Retrieve the weapon explicitly tracked by the character
    UTP_WeaponComponent* WeaponComp = Player->GetCurrentWeapon();
    
    // Fallback: Try finding component if GetCurrentWeapon returns null (Double check)
    if (!WeaponComp)
    {
         WeaponComp = Player->FindComponentByClass<UTP_WeaponComponent>();
    }

    UStatusComponent* StatusComp = Player->GetStatusComponent();

    if (!WeaponComp)
    {
        SetInfoText(TEXT("No Weapon\nEquipped"), FColor::Red);
        if (UpgradeFailSound) UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
        return;
    }

    if (!StatusComp)
    {
         // Should rarely happen for RoboQuestCharacter
         return;
    }

    // 2. Calculate Cost (Cost == Current Weapon Level)
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
        SetInfoText(SuccessMsg, FColor::Green);

		UE_LOG(LogTemp, Log, TEXT("WeaponUpgradeBench::Weapon upgraded from Level %d to Level %d for %d Power Cells."), OldLevel, NewLevel, UpgradeCost);
        
        if (UpgradeSuccessSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, UpgradeSuccessSound, GetActorLocation());
        }
        else if (WeaponComp->ReloadSound)
        {
            // Fallback sound check
            UGameplayStatics::PlaySoundAtLocation(this, WeaponComp->ReloadSound, GetActorLocation());
        }
    }
    else
    {
        // Not enough currency
        FString FailMsg = FString::Printf(TEXT("Insuffient Power!\nCost: %d / Have: %d"), UpgradeCost, CurrentPower);
        SetInfoText(FailMsg, FColor::Red);

        if (UpgradeFailSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, UpgradeFailSound, GetActorLocation());
        }
    }
}

void AWeaponUpgradeBench::SetInfoText(FString NewText, FColor Color)
{
	if (InfoTextRender)
	{
		InfoTextRender->SetText(FText::FromString(NewText));
		InfoTextRender->SetTextRenderColor(Color);
	}
}

