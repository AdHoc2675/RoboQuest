// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "RoboQuestCharacter.h"
#include "RoboQuestProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" // Required for VRandCone and Math functions
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
	
	// Default Fallback values
	CurrentAmmo = MaxAmmo;
    
    bFireInputHeld = false;
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
	// 1. Validation Check
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// 2. Cooldown Check (Rate of Fire)
	double CurrentTime = GetWorld()->GetTimeSeconds();
	float FireDelay = (RateOfFire > 0) ? (1.0f / RateOfFire) : 0.1f;
	
	if (CurrentTime - LastFireTime < FireDelay - 0.01f)
	{
		return; 
	}

	// 3. Ammo & Reload Check
	if (!CanFire())
	{
		// Auto reload if out of ammo and not currently reloading
		if (CurrentAmmo <= 0 && !bIsReloading)
		{
			Reload();
		}
        
        StopAutomaticFire();
		return;
	}

	// 4. Update Ammo
	LastFireTime = CurrentTime;
	CurrentAmmo--;
	
	// Notify ammo change UI
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}

	// 5. Fire Logic (Updated: Converging Aim + Spread + Recoil)
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

		if (World != nullptr && PlayerController != nullptr)
		{
			// --- A. Converging Aim Logic (Find where the crosshair is pointing) ---
			FVector CameraLoc;
			FRotator CameraRot;
			PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

			FVector TraceStart = CameraLoc;
			FVector TraceEnd = CameraLoc + (CameraRot.Vector() * 10000.0f); // Trace 100m forward

			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner()); // Ignore self

			// Perform Line Trace to find target point in center of screen
			bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
			FVector TargetLocation = bHit ? Hit.Location : TraceEnd;

			// --- B. Determine Muzzle Location ---
			FVector MuzzleLoc = GetOwner()->GetActorLocation();
			if (DoesSocketExist(TEXT("Muzzle")))
			{
				MuzzleLoc = GetSocketLocation(TEXT("Muzzle"));
			}
			else
			{
				// Fallback to offset if no socket
				MuzzleLoc = GetOwner()->GetActorLocation() + CameraRot.RotateVector(MuzzleOffset);
			}

			// --- C. Spawn Projectiles (Loop for Shotguns) ---
			for(int32 i = 0; i < BulletCount; i++)
			{
				// Calculate direction from Muzzle to the Target Point
				FVector DirectionToTarget = (TargetLocation - MuzzleLoc).GetSafeNormal();

				// Apply Bullet Spread (AimVariance)
				FVector SpreadDirection = FMath::VRandCone(DirectionToTarget, FMath::DegreesToRadians(AimVariance));
				FRotator SpawnRotation = SpreadDirection.Rotation();
		
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ActorSpawnParams.Owner = Character;
				ActorSpawnParams.Instigator = Character;

                // Spawn Projectile
				ARoboQuestProjectile* Projectile = World->SpawnActor<ARoboQuestProjectile>(ProjectileClass, MuzzleLoc, SpawnRotation, ActorSpawnParams);
				
				if (Projectile)
				{
					Projectile->InitializeProjectile(Damage, RangeMeter, CritDamageMultiplier);
				}
			}

			// --- D. Apply Recoil ---
			if (RecoilStrength > 0.0f)
			{
				// Randomize recoil slightly for realism
				float RecoilPitch = -RecoilStrength * FMath::RandRange(0.4f, 0.6f); // Kick up
				float RecoilYaw = RecoilStrength * FMath::RandRange(-0.25f, 0.25f);   // Shake left/right

				PlayerController->AddPitchInput(RecoilPitch);
				PlayerController->AddYawInput(RecoilYaw);
			}
		}
	}
	
	// Effects
	// Play Sound
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Play Animation
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

	// Bind weapon to HUD
	Character->BindWeaponToHUD(this);

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
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::Reload);
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
    // [Added] Track input state
    bFireInputHeld = true;

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

// Input Released
void UTP_WeaponComponent::StopFire()
{
    // [Added] User released the button
    bFireInputHeld = false;
    StopAutomaticFire();
}

// Internal Helper
void UTP_WeaponComponent::StopAutomaticFire()
{
    GetWorld()->GetTimerManager().ClearTimer(AutomaticFireTimer);
}

void UTP_WeaponComponent::Reload()
{
    // [Modified] Stop timer but keep 'bFireInputHeld' true if key is held
    StopAutomaticFire();

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

    // [Added] Resume Firing if button is still held
    if (bFireInputHeld)
    {
        StartFire();
    }
}