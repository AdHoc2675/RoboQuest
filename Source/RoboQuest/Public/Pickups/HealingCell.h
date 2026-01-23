// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealingCell.generated.h"

UCLASS()
class ROBOQUEST_API AHealingCell : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealingCell();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	// --- Config ---
	// Amount of health to restore
	UPROPERTY(EditAnywhere, Category = "Healing")
	float HealAmount = 4.0f;

	// Distance to start flying towards the player
	UPROPERTY(EditAnywhere, Category = "Healing")
	float MagnetDetectRange = 400.0f;

	// Flying speed towards the player
	UPROPERTY(EditAnywhere, Category = "Healing")
	float MagnetFlySpeed = 750.0f;

	// Initial physics impulse force when spawned
	UPROPERTY(EditAnywhere, Category = "Healing")
	float SpawnImpulseStrength = 150.0f;

private:
	// State
	bool bIsMagnetized = false;
	// Prevent double consumption
	bool bIsConsumed = false;
	AActor* TargetPlayer = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
