// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/SlidingDoor.h"
#include "StageFinishDoor.generated.h"

class UStageResultWidget;

/**
 * AStageFinishDoor: A specialized door that ends the stage.
 * Instead of opening physically, it triggers a fade-out sequence and shows the result UI.
 */
UCLASS()
class ROBOQUEST_API AStageFinishDoor : public ASlidingDoor
{
	GENERATED_BODY()
	
public:
	AStageFinishDoor();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() override;

protected:
	// --- Config ---

	// Widget class to spawn for results
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage Finish")
	TSubclassOf<UStageResultWidget> ResultWidgetClass;

	// Duration of the screen fade out (Camera fade)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage Finish")
	float FadeOutDuration = 2.0f;

	// Name of this stage to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage Finish")
	FName StageDisplayName = TEXT("Oasis");

	// --- State ---

	bool bFinishSequenceStarted = false;

	// Handle for sequencing events
	FTimerHandle SequenceTimerHandle;

	// 1. Start the fade out (Audio & Video)
	void StartFinishSequence(class AController* PlayerController);

	// 2. Show UI after fade is done
	void ShowResultUI();
	
	// Cache the controller for the timer callback
	UPROPERTY()
	APlayerController* CachedPC;
};
