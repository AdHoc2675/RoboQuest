// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/StatusComponent.h"
#include "UI/BaseUserHUDWidget.h"
#include "RoboQuestCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARoboQuestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Status Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	UStatusComponent* StatusComponent;
	
public:
	ARoboQuestCharacter();



protected:
	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBaseUserHUDWidget> HUDWidgetClass;

	UPROPERTY()
	UBaseUserHUDWidget* HUDWidget;

	bool bIsDead = false;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns StatusComponent subobject **/
	UStatusComponent* GetStatusComponent() const { return StatusComponent; }
	/** Binds the given weapon component to the HUD */
	void BindWeaponToHUD(class UTP_WeaponComponent* WeaponComp);
	/** Returns whether the character is alive */
	bool IsAlive() const { return !bIsDead; }
	/** Override TakeDamage to apply damage to StatusComponent */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

