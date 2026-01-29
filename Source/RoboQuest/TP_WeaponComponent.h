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

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ReloadAction;

	/** Delegate for ammo update events */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmoChanged OnAmmoChanged;

	// --- Weapon Stats (From DataTable) ---

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Damage = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 BulletCount = 1;

	// Fire rate (Rounds Per Second)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float RateOfFire = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MaxAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float RangeMeter = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float ReloadTime = 1.5f;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CritDamageMultiplier = 1.5f;

	/** Cone half-angle for variance while aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Accuracy")
	float AimVariance = 0.5f;

	/** Amount of firing recoil to apply to the owner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Recoil")
	float RecoilStrength = 0.5f;

	// Enum Stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	EAmmoType AmmoType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	EWeaponType WeaponType;

	// --- Config ---

	/** The DataTable used to initialize this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Data")
	UDataTable* WeaponDataTable;

	/** The Row Name to look up in the DataTable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Data")
	FName WeaponRowName;

	// --- Functions ---

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

protected:
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
