// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "SlidingDoor.generated.h"

class UStaticMeshComponent;

/**
 * A door that uses a Static Mesh and slides simply (Interpolation) without animations.
 * Default behavior is sliding UP on the Z-axis.
 */
UCLASS()
class ROBOQUEST_API ASlidingDoor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    ASlidingDoor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- Components ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootScene;

    // The visual mesh of the door
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DoorMesh;

    // --- Configuration ---

    // The offset to apply when opening. Default is (0, 0, 300) to slide up.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    FVector OpenOffset;

    // How fast the door moves
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    float MoveSpeed;

    // Is the door currently locked?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
    bool bIsLocked;

    // --- IInteractable Interface ---

    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() override;

    // --- API ---

    // Manually lock or unlock the door
    UFUNCTION(BlueprintCallable, Category = "Door")
    void SetLocked(bool bLocked);

    // Manually open or close the door
    UFUNCTION(BlueprintCallable, Category = "Door")
    void SetDoorState(bool bOpen);

private:
    bool bIsOpen;

    // Stores the initial location of the mesh (Closed state)
    FVector ClosedRelativeLocation;

    // The current target location for interpolation
    FVector TargetRelativeLocation;
};