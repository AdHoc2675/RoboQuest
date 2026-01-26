// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "DoorBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ROBOQUEST_API ADoorBase : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    ADoorBase();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* RootScene;

    // The door mesh itself (movable part)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DoorMesh;

    // Simple timeline/animation support via Curve (Timeline is better in BP, but we can use simple interpolation or TimelineComponent in C++)
    // For simplicity with Animations mentioned:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    class UAnimationAsset* OpenAnimation; // Or UCurveFloat for sliding doors

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    class UAnimationAsset* CloseAnimation;

    bool bIsOpen;

public:
    // Implementation of the Interact method from IInteractable
    virtual void Interact_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Door")
    void PlayOpenEffects();

    UFUNCTION(BlueprintImplementableEvent, Category = "Door")
    void PlayCloseEffects();
};