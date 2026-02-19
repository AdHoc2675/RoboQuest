// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/StageFinishDoor.h"
#include "UI/StageResultWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"

AStageFinishDoor::AStageFinishDoor()
{
	// Override defaults from SlidingDoor if needed
	bIsLocked = false; 
	FadeOutDuration = 1.5f;
}

FText AStageFinishDoor::GetInteractionPrompt_Implementation()
{
	return FText::FromString(TEXT("[E] Finish Stage"));
}

void AStageFinishDoor::Interact_Implementation(AActor* Interactor)
{
	if (bFinishSequenceStarted) return;

	APawn* Pawn = Cast<APawn>(Interactor);
	if (!Pawn) return;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;

	bFinishSequenceStarted = true;
	CachedPC = PC;

	// 1. Play Door Sound (Optional, maybe an "Unlock" sound instead of sliding)
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	StartFinishSequence(PC);
}

void AStageFinishDoor::StartFinishSequence(AController* PlayerController)
{
	APlayerController* PC = Cast<APlayerController>(PlayerController);
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	// 1. Disable Input
	Pawn->DisableInput(PC);

	// 2. Screen Fade Out (to Black)
	if (PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeOutDuration, FLinearColor::Black, false, true);
	}

	// 3. Audio Fade Out

	// UGameplayStatics::PushSoundMixModifier(this, FadeOutMix); 

	UE_LOG(LogTemp, Log, TEXT("StageFinishDoor: Sequence Started..."));

	// 4. Set Timer to Show UI
	GetWorld()->GetTimerManager().SetTimer(SequenceTimerHandle, this, &AStageFinishDoor::ShowResultUI, FadeOutDuration + 0.5f, false);
}

void AStageFinishDoor::ShowResultUI()
{
	if (!CachedPC || !ResultWidgetClass) return;

	// 1. Create Widget
	UStageResultWidget* ResultWidget = CreateWidget<UStageResultWidget>(CachedPC, ResultWidgetClass);
	if (ResultWidget)
	{
		// 2. Calculate Stage Time
		// Since World Time resets on Level Load, GetTimeSeconds() is exactly the duration of this run.
		float FinalTime = GetWorld()->GetTimeSeconds();
		
		// TODO: Retrieve actual Enemy Kill Count & Rank logic later.
		int32 MockEnemies = FMath::RandRange(10, 50); 
		FString MockRank = TEXT("S"); 
		if (FinalTime > 180.0f) MockRank = TEXT("A");
		if (FinalTime > 300.0f) MockRank = TEXT("B");

		ResultWidget->SetupResultData(StageDisplayName, FinalTime, MockEnemies, MockRank);

		// 3. Add to Viewport
		ResultWidget->AddToViewport(100); // High Z-order

		// 4. Setup Input for UI
		CachedPC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(ResultWidget->TakeWidget());
		CachedPC->SetInputMode(InputMode);

		UE_LOG(LogTemp, Log, TEXT("StageFinishDoor: Result UI Shown. Time: %.2f"), FinalTime);
	}
}


