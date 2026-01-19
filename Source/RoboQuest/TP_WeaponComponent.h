// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	// --- Ammo & Reload System ---

	/** Maximum ammo capacity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Ammo")
	int32 MaxAmmo = 30;

	/** Current ammo count */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Ammo")
	int32 CurrentAmmo;

	/** Time required to reload (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Ammo")
	float ReloadTime = 1.5f;

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

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	ARoboQuestCharacter* Character;
};
