// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "RoboQuestCharacter.h"
#include "RoboQuestProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "TimerManager.h" 

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	CurrentAmmo = MaxAmmo;
}

void UTP_WeaponComponent::InitializeWeapon(FName NewWeaponRowName)
{
	// Ensure DataTable is valid
	if (!WeaponDataTable)
	{
		// Fallback: If no table, just reset ammo
		CurrentAmmo = MaxAmmo;
		return;
	}

	static const FString ContextString(TEXT("Weapon Initialization"));
	FWeaponStatRow* Row = WeaponDataTable->FindRow<FWeaponStatRow>(NewWeaponRowName, ContextString);

	if (Row)
	{
		WeaponRowName = NewWeaponRowName;

		// Apply Stats from DataTable
		Damage = Row->Damage;
		BulletCount = Row->BulletCount;
		RateOfFire = Row->RateOfFire; // e.g., 5.0 (shots per sec)
		MaxAmmo = Row->Capacity;
		RangeMeter = Row->RangeMeter;
		ReloadTime = Row->ReloadTime;
		CritDamageMultiplier = Row->CritDamage;
		
		// Apply Enums
		AmmoType = Row->AmmoType;
		WeaponType = Row->WeaponType;

		// Reset State
		CurrentAmmo = MaxAmmo;
		bIsReloading = false;

		// Notify UI
		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
		}
	}
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	double CurrentTime = GetWorld()->GetTimeSeconds();
	float FireDelay = (RateOfFire > 0) ? (1.0f / RateOfFire) : 0.1f;
	
	if (CurrentTime - LastFireTime < FireDelay - 0.01f)
	{
		return; 
	}

	// Check Ammo & Reload
	if (!CanFire())
	{
		// Auto reload if out of ammo and not currently reloading
		if (CurrentAmmo <= 0 && !bIsReloading)
		{
			Reload();
		}
        // Stop timer if firing is not possible (e.g., out of ammo)
        StopFire();
		return;
	}

	// Mark Fire Time & Consume Ammo
	LastFireTime = CurrentTime;
	CurrentAmmo--;
	
	// Notify ammo change
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}

	// Spawn Projectile(s)
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			
			// We fire 'BulletCount' projectiles per shot
			for(int32 i = 0; i < BulletCount; i++)
			{
				FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				
				// Optional: Add Spread here if BulletCount > 1
				// if (BulletCount > 1) { SpawnRotation += RandomSpread... }

				const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		
				// Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
				// Spawn the projectile
				ARoboQuestProjectile* Projectile = World->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				
				// Initialize projectile's properties
				if (Projectile)
				{
					Projectile->InitializeProjectile(Damage, RangeMeter, CritDamageMultiplier);
				}
			}
		}
	}
	
	// Effects
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

bool UTP_WeaponComponent::AttachWeapon(ARoboQuestCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
            // Changed Triggered -> Started & Completed
            // When press started (Started) -> StartFire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartFire);
            
            // When released (Completed) -> StopFire
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopFire);

			// Bind Reload Action
			if (ReloadAction)
			{
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Reload);
			}
		}
	}

	// Initialize Stats on Attachment (Optional: could also be done on BeginPlay)
	if(!WeaponRowName.IsNone())
	{
		InitializeWeapon(WeaponRowName);
	}
	else
	{
		// Fallback notify
		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

bool UTP_WeaponComponent::CanFire() const
{
	// Can fire if character is valid, has ammo, and is not reloading
	return (Character != nullptr) && (CurrentAmmo > 0) && (!bIsReloading);
}

void UTP_WeaponComponent::StartFire()
{
    double CurrentTime = GetWorld()->GetTimeSeconds();
    float FireDelay = (RateOfFire > 0) ? (1.0f / RateOfFire) : 0.1f;

    // Ignore if last fire was within FireDelay
    if (CurrentTime - LastFireTime < FireDelay)
    {
        return; 
    }

    Fire();

    // Set timer according to rate of fire (automatic fire) 
    if (RateOfFire > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(AutomaticFireTimer, this, &UTP_WeaponComponent::Fire, FireDelay, true);
    }
}

// Stop firing
void UTP_WeaponComponent::StopFire()
{
    GetWorld()->GetTimerManager().ClearTimer(AutomaticFireTimer);
}

void UTP_WeaponComponent::Reload()
{
    // Stop firing when reload starts
    StopFire();

	// Check conditions: Ignore if already reloading or ammo is full
	if (bIsReloading || CurrentAmmo >= MaxAmmo)
	{
		return;
	}

	bIsReloading = true;

	// Play reload animation
	if (ReloadAnimation && Character)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadAnimation);
		}
	}
	
	// Start timer to finish reloading
	FTimerHandle ReloadTimerHandle;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UTP_WeaponComponent::FinishReloading, ReloadTime, false);
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Reloading started..."));
}

void UTP_WeaponComponent::FinishReloading()
{
	bIsReloading = false;
	CurrentAmmo = MaxAmmo; // Refill ammo completely
    
	// Notify UI regarding full ammo
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Reloading finished. Ammo refilled."));
}