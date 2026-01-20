// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusComponent.h"
#include "EnemyBase.generated.h"

UCLASS()
class ROBOQUEST_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyHealthComponent* Health;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyTargetComponent* Target;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyDropComponent* Drop;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Components")
	//UEnemyWeaponComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStatusComponent* StatusComponent;

	//// --- Data ---
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Data")
	//UEnemyArchetypeDataAsset* Archetype;

	bool bIsDead = false;

public:	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const { return !bIsDead; }

protected:
	// bind to health changed event
	UFUNCTION()
	void OnHealthChanged(float CurrentHealth, float ScratchHealth, float MaxHealth);
};
