// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerCell.generated.h"

// Drop item that grants "Power" (Currency for Weapon Upgrades)
UCLASS()
class ROBOQUEST_API APowerCell : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerCell();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// --- Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	// --- Config ---
	// Amount of Power (Currency) to grant
	UPROPERTY(EditAnywhere, Category = "Power")
	int32 PowerAmount = 1;

	// Distance to start flying towards the player
	UPROPERTY(EditAnywhere, Category = "Power")
	float MagnetDetectRange = 500.0f;

	// Flying speed towards the player
	UPROPERTY(EditAnywhere, Category = "Power")
	float MagnetFlySpeed = 1600.0f;

	// Initial physics impulse force when spawned
	UPROPERTY(EditAnywhere, Category = "Power")
	float SpawnImpulseStrength = 180.0f;

private:
	// State
	bool bIsMagnetized = false;
	// Prevent double consumption
	bool bIsConsumed = false;

	AActor* TargetPlayer = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
