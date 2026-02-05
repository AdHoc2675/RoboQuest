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
#include "Data/WeaponAffix.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Enable Tick needed for smooth crosshair recovery
	PrimaryComponentTick.bCanEverTick = true; // Tick 활성화
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	
	// Default Fallback values
	CurrentAmmo = BaseMaxAmmo;
    
    bFireInputHeld = false;

	CurrentSpread = BaseMinSpread;
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpread = BaseMinSpread;
}

void UTP_WeaponComponent::UpgradeWeapon()
{
	// Increase Weapon Level
	WeaponLevel++;

	RecalculateStats();

	// Refill Ammo as a bonus
	CurrentAmmo = BaseMaxAmmo;

	// Notify UI
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, BaseMaxAmmo);
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Weapon Upgraded. New Level: %d, New Damage: %f"), WeaponLevel, FinalDamage);
}

void UTP_WeaponComponent::AddAffix(TSubclassOf<UWeaponAffix> AffixClass)
{
	if (!AffixClass) return;

	// Check Max Slots
	if (CurrentAffixes.Num() >= GetMaxAffixCount())
	{
		UE_LOG(LogTemp, Warning, TEXT("UTP_WeaponComponent::Cannot add affix: Max slots reached for current rarity."));
		return;
	}

	// Create Instance
	UWeaponAffix* NewAffix = NewObject<UWeaponAffix>(this, AffixClass);
	if (NewAffix)
	{
		CurrentAffixes.Add(NewAffix);
		RecalculateStats(); // Re-apply stats immediately
	}
}

void UTP_WeaponComponent::RecalculateStats()
{
	// Reset Modifiers
	DamageMultiplier = 1.0f;
	RangeMultiplier = 1.0f;
	RateOfFireMultiplier = 1.0f;
	MaxAmmoMultiplier = 1.0f;
	ReloadTimeMultiplier = 1.0f;
	SpreadMultiplier = 1.0f;
	CurrentSpeedBonus = 0.0f;

	// Apply Level Scaling
	// Example: +10% damage per level
	if (WeaponLevel > 1)
	{
		DamageMultiplier += (WeaponLevel - 1) * 0.1f;
		RangeMultiplier += (WeaponLevel - 1) * 0.1f;
	}

	// Apply Affix Modifiers
	for (UWeaponAffix* Affix : CurrentAffixes)
	{
		if (Affix)
		{
			Affix->ApplyStatModifiers(this);
		}
	}

	// Calculate Final Stats
	FinalDamage = BaseDamage * DamageMultiplier; // Update the variable used in logic
	FinalRangeMeter = BaseRangeMeter * RangeMultiplier;
	FinalRateOfFire = BaseRateOfFire * RateOfFireMultiplier;

	FinalMaxAmmo = FMath::Max(1, FMath::RoundToInt(BaseMaxAmmo * MaxAmmoMultiplier));
	if (CurrentAmmo > FinalMaxAmmo) CurrentAmmo = FinalMaxAmmo;

	// Avoid divide by zero
	float SafeReloadSpeed = FMath::Max(0.1f, ReloadTimeMultiplier);
	FinalReloadTime = BaseReloadTime / SafeReloadSpeed;

	// Lower SpreadMultiplier means more accurate. Higher means less accurate.
	FinalMinSpread = BaseMinSpread * SpreadMultiplier;
	FinalMaxSpread = BaseAimVariance * SpreadMultiplier;
	FinalAimVariance = BaseAimVariance * SpreadMultiplier;
	if (CurrentSpread < BaseMinSpread) CurrentSpread = BaseMinSpread;

	// Update Speed Bonus on Character
	if (Character)
	{
		if (UStatusComponent* Status = Character->GetStatusComponent())
		{
			// Remove previous bonus and add new one
			// Status->AddSpeed(-LastAppliedSpeedBonus); 
			// Status->AddSpeed(CurrentSpeedBonus);

			// Or calculate Delta
			float SpeedDelta = CurrentSpeedBonus - LastAppliedSpeedBonus;
			if (!FMath::IsNearlyZero(SpeedDelta))
			{
				Status->AddSpeed(SpeedDelta);
				LastAppliedSpeedBonus = CurrentSpeedBonus;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Stats Recalculated."));
}

int32 UTP_WeaponComponent::GetMaxAffixCount() const
{
	// Rarity determines max slots
	switch (WeaponRarity)
	{
	case EWeaponRarity::Common:    return 1;
	case EWeaponRarity::Uncommon:  return 2;
	case EWeaponRarity::Rare:      return 3;
	case EWeaponRarity::Epic:      return 4;
	case EWeaponRarity::Fantastic: return 5;
	default: return 1;
	}
}

// Tick 함수 구현
void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. 탄퍼짐 회복 (사격 중이 아닐 때 혹은 항상)
	// 목표: MinSpread로 서서히 돌아감
	if (CurrentSpread > BaseMinSpread)
	{
		CurrentSpread = FMath::FInterpTo(CurrentSpread, BaseMinSpread, DeltaTime, SpreadRecoveryRate);
	}
	
	// 2. HUD 업데이트
	if (Character)
	{
		if (UBaseUserHUDWidget* HUD = Character->GetHUDWidget())
		{
			HUD->UpdateCrosshairSpread(CurrentSpread);
		}
	}
}

void UTP_WeaponComponent::InitializeWeapon(FName NewWeaponRowName)
{
	// Ensure DataTable is valid
	if (!WeaponDataTable)
	{
		// Fallback: If no table, just reset ammo
		CurrentAmmo = BaseMaxAmmo;
		return;
	}

	static const FString ContextString(TEXT("Weapon Initialization"));
	FWeaponStatRow* Row = WeaponDataTable->FindRow<FWeaponStatRow>(NewWeaponRowName, ContextString);

	if (Row)
	{
		WeaponRowName = NewWeaponRowName;

		// Apply Stats from DataTable
		BaseDamage = Row->Damage;
		BulletCount = Row->BulletCount;
		BaseRateOfFire = Row->RateOfFire; // e.g., 5.0 (shots per sec)
		BaseMaxAmmo = Row->Capacity;
		BaseRangeMeter = Row->RangeMeter;
		BaseReloadTime = Row->ReloadTime;
		BaseCritDamageMultiplier = Row->CritDamage;
		
		// Apply Enums
		AmmoType = Row->AmmoType;
		WeaponType = Row->WeaponType;

		// Apply Accuracy Stats
		BaseAimVariance = Row->AimVariance;
		BaseMinSpread = Row->AimVariance;
		
		CurrentSpread = BaseMinSpread;

		CurrentAffixes.Empty();
		for (const TSubclassOf<UWeaponAffix>& AffixClass : Row->DefaultAffixes)
		{
			AddAffix(AffixClass);
		}

		if (Row->WeaponMesh)
		{
			SetSkeletalMesh(Row->WeaponMesh);
		}

		FireAnimation = Row->CharacterFireAnim;
		ReloadAnimation = Row->CharacterReloadAnim;
		WeaponFireAnimation = Row->WeaponFireAnim;
		WeaponReloadAnimation = Row->WeaponReloadAnim;

		// Reset State
		CurrentAmmo = BaseMaxAmmo;
		bIsReloading = false;

		// Notify UI
		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, BaseMaxAmmo);
		}

		RecalculateStats();
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Weapon Initialized: %s"), *NewWeaponRowName.ToString());
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
	float FireDelay = (BaseRateOfFire > 0) ? (1.0f / BaseRateOfFire) : 0.1f;
	
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
		OnAmmoChanged.Broadcast(CurrentAmmo, BaseMaxAmmo);
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

				// Apply Bullet Spread (AimVariance 대신 CurrentSpread 사용!)
				FVector SpreadDirection = FMath::VRandCone(DirectionToTarget, FMath::DegreesToRadians(CurrentSpread));
				FRotator SpawnRotation = SpreadDirection.Rotation();
		
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ActorSpawnParams.Owner = Character;
				ActorSpawnParams.Instigator = Character;

                // Spawn Projectile
				ARoboQuestProjectile* Projectile = World->SpawnActor<ARoboQuestProjectile>(ProjectileClass, MuzzleLoc, SpawnRotation, ActorSpawnParams);
				
				if (Projectile)
				{
					Projectile->InitializeProjectile(FinalDamage, FinalRangeMeter, BaseCritDamageMultiplier);
				}
			}

			// --- D. Apply Recoil ---
			if (BaseRecoilStrength > 0.0f)
			{
				// Randomize recoil slightly for realism
				float RecoilPitch = -BaseRecoilStrength * FMath::RandRange(0.4f, 0.6f); // Kick up
				float RecoilYaw = BaseRecoilStrength * FMath::RandRange(-0.25f, 0.25f);   // Shake left/right

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

	// Play Weapon Animation
	if (WeaponFireAnimation != nullptr)
	{
		// WeaponComponent 자체가 SkeletalMeshComponent이므로 자신의 AnimInstance를 가져옵니다.
		UAnimInstance* WeaponAnimInstance = GetAnimInstance();
		if (WeaponAnimInstance != nullptr)
		{
			WeaponAnimInstance->Montage_Play(WeaponFireAnimation, 1.f);
		}
	}

	// Increase Spread on Fire
	CurrentSpread = FMath::Min(CurrentSpread + SpreadIncreasePerShot, BaseMaxSpread);
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

		UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Weapon Attached: %s"), *WeaponRowName.ToString());
	}
	else
	{
		// Fallback notify
		if (OnAmmoChanged.IsBound())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, BaseMaxAmmo);
		}

		UE_LOG(LogTemp, Warning, TEXT("UTP_WeaponComponent::Weapon Attached with no WeaponRowName set."));
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
    float FireDelay = (BaseRateOfFire > 0) ? (1.0f / BaseRateOfFire) : 0.1f;

    // Ignore if last fire was within FireDelay
    if (CurrentTime - LastFireTime < FireDelay)
    {
        return; 
    }

    Fire();

    // Set timer according to rate of fire (automatic fire) 
    if (BaseRateOfFire > 0.0f)
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
	if (bIsReloading || CurrentAmmo >= BaseMaxAmmo)
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

	// Play weapon reload animation
	if (WeaponReloadAnimation != nullptr)
	{
		UAnimInstance* WeaponAnimInstance = GetAnimInstance();
		if (WeaponAnimInstance != nullptr)
		{
			WeaponAnimInstance->Montage_Play(WeaponReloadAnimation);
		}
	}

	// Play reload sound
	if (ReloadSound != nullptr && Character)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, Character->GetActorLocation());
	}
	
	// Start timer to finish reloading
	FTimerHandle ReloadTimerHandle;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UTP_WeaponComponent::FinishReloading, BaseReloadTime, false);
	}

	UE_LOG(LogTemp, Log, TEXT("UTP_WeaponComponent::Reloading started..."));
}

void UTP_WeaponComponent::FinishReloading()
{
	bIsReloading = false;
	CurrentAmmo = BaseMaxAmmo; // Refill ammo completely
    
	// Notify UI regarding full ammo
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, BaseMaxAmmo);
	}

    // [Added] Resume Firing if button is still held
    if (bFireInputHeld)
    {
        StartFire();
    }
}