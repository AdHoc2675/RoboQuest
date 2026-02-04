// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WeaponUpgradeBench.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

/**
 * A robust weapon upgrade station logic.
 * Consumes Power Cells based on the weapon's current level to upgrade stats.
 */
UCLASS()
class ROBOQUEST_API AWeaponUpgradeBench : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AWeaponUpgradeBench();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame to handle text reset logic
	virtual void Tick(float DeltaTime) override;

    // Interface implementation
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BenchMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UTextRenderComponent* InfoTextRender;

	// Sound to play when upgrade is successful
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* UpgradeSuccessSound;

    // Sound to play when upgrade fails
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* UpgradeFailSound;

private:
	// Time when the interaction last occurred, used to reset text
	double LastInteractTime = -10.0;

	// Helper to update the text display
	void SetInfoText(FString NewText, FColor Color = FColor::White);

	// Default message to display
    const FString DefaultMessage = TEXT("Weapon Upgrade\n[ Interact (E) ]");
};
