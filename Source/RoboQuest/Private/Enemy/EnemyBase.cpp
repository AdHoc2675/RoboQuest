// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusComponent.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "UI/DamageTextWidget.h"
#include "Engine/DamageEvents.h"
#include "Data/ElementalDamageTypes.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StatusComponent2 = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// bind to health changed event
    if (StatusComponent2)
    {
        StatusComponent2->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnHealthChanged);
    }
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// apply damage to status component
    if (StatusComponent2 && IsAlive())
    {
        StatusComponent2->TakeDamage(ActualDamage);
        
		if (ActualDamage > 0.0f)
		{
			// bool bIsCritical = (ActualDamage > 20.0f); 
            bool bIsCritical = false;
            // Determine Color based on Damage Type
            FLinearColor DmgColor = FLinearColor::White;
            if (DamageEvent.DamageTypeClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("EnemyBase::TakeDamage Type: %s"), *DamageEvent.DamageTypeClass->GetName());

                if (DamageEvent.DamageTypeClass->IsChildOf(UDamageTypeBurn::StaticClass()))
                {
                    DmgColor = FLinearColor(1.0f, 0.2f, 0.0f); // Burn: Orange-Red
                }
                else if (DamageEvent.DamageTypeClass->IsChildOf(UDamageTypeCryo::StaticClass()))
                {
                    DmgColor = FLinearColor(0.2f, 0.8f, 1.0f); // Cryo: Sky Blue
                }
                else if (DamageEvent.DamageTypeClass->IsChildOf(UDamageTypeShock::StaticClass()))
                {
                    DmgColor = FLinearColor(0.6f, 0.2f, 1.0f); // Shock: Purple
                }
            }

			ShowFloatingDamage(ActualDamage, bIsCritical, DmgColor);
		}
		// -----------------------------

		// If there is an aggro system, set the DamageCauser as the target here

        // can add additional reactions to health changes here (e.g., play hurt animations, sounds, etc.)
    }

    return ActualDamage;
}

void AEnemyBase::OnHealthChanged(float CurrentHealth, float ScratchHealth, float MaxHealth)
{
	// Check for death
    if (CurrentHealth <= 0.0f && IsAlive())
    {
        Die();
    }

}

void AEnemyBase::Die()
{
    if (bIsDead) return;
    
    bIsDead = true;

    SpawnDrops();

	// give exp to player
    ARoboQuestCharacter* PlayerCharacter = Cast<ARoboQuestCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && StatusComponent2)
    {
        PlayerCharacter->GetStatusComponent()->AddExp(StatusComponent2->ExpReward);

        // Notify player of takedown
        if (PlayerCharacter->OnEnemyKilled.IsBound())
        {
            PlayerCharacter->OnEnemyKilled.Broadcast();
        }
	}

	// Disable collisions
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); // NoCollision

    GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	// enable ragdoll physics
    GetMesh()->SetSimulatePhysics(true);

    // Detach controller
    DetachFromControllerPendingDestroy();

    // Destroy actor after a delay (set LifeSpan)
    SetLifeSpan(5.0f); 
}

void AEnemyBase::SpawnDrops()
{
    if (!HealingCellClass || !PowerCellClass) return;

    // Healing Cells (Guaranteed drop based on Count)
    if (HealingCellClass)
    {
        for (int32 i = 0; i < DropCount; i++)
        {
            // Random Spawn Position around the enemy
            FVector SpawnLoc = GetActorLocation() + FMath::VRand() * 20.0f;
            SpawnLoc.Z += 50.0f; // Drop from body height

            FRotator SpawnRot = FMath::VRand().Rotation();

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            Params.Owner = this;

            GetWorld()->SpawnActor<AActor>(HealingCellClass, SpawnLoc, SpawnRot, Params);
        }
    }

    // Power Cells (Probabilistic Drop for Upgrade Currency)
    if (PowerCellClass)
    {
        // Check Probability (0.0 ~ 1.0)
        if (FMath::FRand() <= PowerDropChance)
        {
            for (int32 i = 0; i < PowerDropCount; i++)
            {
                FVector SpawnLoc = GetActorLocation() + FMath::VRand() * 25.0f;
                SpawnLoc.Z += 60.0f; // Drop from body height

                FRotator SpawnRot = FMath::VRand().Rotation();

                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                Params.Owner = this;

                GetWorld()->SpawnActor<AActor>(PowerCellClass, SpawnLoc, SpawnRot, Params);
            }
        }
    }
}

void AEnemyBase::ShowFloatingDamage(float Damage, bool bCritical, FLinearColor TextColor)
{
	if (!DamageTextWidgetClass) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	// Ensure we have a valid controller
	if (!PC) return;

	// Create Widget
	UDamageTextWidget* DamageWidget = CreateWidget<UDamageTextWidget>(PC, DamageTextWidgetClass);
	if (DamageWidget)
	{
		// Set Location (Slightly above the enemy with some horizontal randomness)
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += FMath::RandRange(50.0f, 100.0f);
		SpawnLocation.Y += FMath::RandRange(-30.0f, 30.0f); // Random horizontal offset

		DamageWidget->PlayDamageText(Damage, SpawnLocation, bCritical, TextColor);
		DamageWidget->AddToViewport();
	}
}