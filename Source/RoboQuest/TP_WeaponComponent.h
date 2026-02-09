// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/WeaponStatRow.h"
#include "TP_WeaponComponent.generated.h"

class ARoboQuestCharacter;

// Delegate to notify when ammo changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOQUEST_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ARoboQuestProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** AnimMontage to play on the weapon each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* WeaponFireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Timer handle for automatic fire */
	FTimerHandle AutomaticFireTimer;

	// --- Ammo & Reload System ---

	/** Current ammo count */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ammo")
	int32 CurrentAmmo;

	/** Is the weapon currently reloading? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ammo")
	bool bIsReloading = false;

	/** Animation to play when reloading */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAnimMontage* ReloadAnimation;

	/** Animation to play on the weapon when reloading */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UAnimMontage* WeaponReloadAnimation;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;

	/** Sound to play when reloading */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadSound;

	/** Delegate for ammo update events */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChanged OnAmmoChanged;

	// --- Weapon Stats (From DataTable) ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseDamage = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float DamageMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float FinalDamage = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 BulletCount = 1;

	// Fire rate (Rounds Per Second)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseRateOfFire = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float RateOfFireMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float FinalRateOfFire = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 BaseMaxAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxAmmoMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 FinalMaxAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseRangeMeter = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float RangeMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float FinalRangeMeter = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseReloadTime = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float ReloadTimeMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float FinalReloadTime = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float BaseCritDamageMultiplier = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CritDamageMultiplierBonus = 0.0f;

	/** Cone half-angle for variance while aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseAimVariance = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float FinalAimVariance = 0.5f;

	/** Amount of firing recoil to apply to the owner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseRecoilStrength = 0.5f;

	// Enum Stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	EAmmoType AmmoType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	EWeaponType WeaponType;

	float CurrentSpeedBonus = 0.0f;
	float LastAppliedSpeedBonus = 0.0f;

	// --- Config ---

	/** The DataTable used to initialize this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Data")
	UDataTable* WeaponDataTable;

	/** The Row Name to look up in the DataTable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Data")
	FName WeaponRowName;

	/** Current Spread Value (Angle in degrees) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Accuracy")
	float CurrentSpread;

	/** Minimum/Base Spread */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float BaseMinSpread = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float FinalMinSpread = 0.5f;

	/** Maximum Spread */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float BaseMaxSpread = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float FinalMaxSpread = 4.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float SpreadMultiplier = 1.0f;

	/** Spread added per shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float SpreadIncreasePerShot = 1.0f;

	/** Spread recovery per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float SpreadRecoveryRate = 5.0f;

	// Dynamic modifiers for Frenzy Ability (Stored here to persist through RecalculateStats)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buffs")
	float FrenzyFireRateMod = 0.0f; // Additive (e.g., 0.1 for +10%)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buffs")
	float FrenzyReloadSpeedMod = 0.0f; // Additive (e.g., 0.1 for +10%)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buffs")
	float FrenzyMoveSpeedMod = 0.0f; // Additive (e.g., 0.05 for +5%)

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool AttachWeapon(ARoboQuestCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	/** Start the reload process */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	/** Called when reload timer finishes */
	void FinishReloading();

	/** Check if weapon can currently fire */
	bool CanFire() const;

	/** Initialize Weapon Stats from DataTable */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InitializeWeapon(FName NewWeaponRowName);

	/** Start automatic fire (Called by Input Started) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	/** Stop automatic fire (Called by Input Completed) */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	// Current Weapon Level (Starts at 1)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Upgrade")
	int32 WeaponLevel = 1;

	// Current Weapon Rarity (Independent of Level)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Upgrade")
	EWeaponRarity WeaponRarity = EWeaponRarity::Common;

	// Apply upgrade: Increase stats and level up
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UpgradeWeapon();

	// --- Affix System ---

	// List of currently active affix instances
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Affix")
	TArray<UWeaponAffix*> CurrentAffixes;

	// Current Damage Type of the weapon (Normal, Burn, Cryo, Shock, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Affix")
	TSubclassOf<UDamageType> CurrentDamageType;

	// Add a new affix to the weapon
	UFUNCTION(BlueprintCallable, Category = "Stats|Affix")
	void AddAffix(TSubclassOf<UWeaponAffix> AffixClass, bool bForce = false);

	// Clears ONLY random affixes (keeps Defaults) and rolls new ones based on Rarity
	UFUNCTION(BlueprintCallable, Category = "Stats|Affix")
	void RerollRandomAffixes();

	// Clear and re-apply all stats (Base + Level + Affixes)
	UFUNCTION(BlueprintCallable, Category = "Stats|Affix")
	void RecalculateStats();

	// Get max number of affixes based on current rarity
	UFUNCTION(BlueprintPure, Category = "Stats|Affix")
	int32 GetMaxAffixCount() const;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Helper to stop the timer without clearing input state (Internal use) */
	void StopAutomaticFire();

private:
	/** The Character holding this weapon*/
	ARoboQuestCharacter* Character;

	/** Last time the weapon was fired (for RateOfFire calculation) */
	double LastFireTime = 0.0;
    
    /** Is the fire input button currently held? */
    bool bFireInputHeld = false;
};
