// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "LevelPortal.generated.h"

class UBoxComponent;
class UTextRenderComponent;

/**
 * An interactive object that triggers a level transition with a fake loading screen.
 */
UCLASS()
class ROBOQUEST_API ALevelPortal : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ALevelPortal();

protected:
	virtual void BeginPlay() override;

public:
	// Name of the level to load (must match the Map filename without extension)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad;

	// Duration of the loading screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	float LoadingDuration = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractionZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PortalMesh;

	// --- IInteractable Interface ---
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() override;
	virtual bool ShouldShowWeaponDetail_Implementation() override { return false; }
};