#include "System/RoboQuestGameInstance.h"
#include "UI/RoboQuestLoadingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void URoboQuestGameInstance::OpenLevelWithFakeLoading(FName LevelName, float FakeDuration)
{
	if (!LoadingWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoboQuestGameInstance::LoadingWidgetClass is NOT set!"));
		UGameplayStatics::OpenLevel(this, LevelName);
		return;
	}

	PendingLevelName = LevelName;
	TargetLoadingTime = FakeDuration;
	CurrentLoadingTime = 0.0f;

	// 1. Create Widget if necessary
	if (!CurrentLoadingWidget)
	{
		CurrentLoadingWidget = CreateWidget<URoboQuestLoadingWidget>(this, LoadingWidgetClass);
	}

	if (CurrentLoadingWidget)
	{
		// 2. Setup Data based on Level Name
		if (LevelLoadingDataMap.Contains(LevelName))
		{
			FLevelLoadingData* Data = LevelLoadingDataMap.Find(LevelName);
			if (Data)
			{
				CurrentLoadingWidget->UpdatePresentation(*Data);
			}
		}
		else
		{
			// Optional: Set default fallback data here
			UE_LOG(LogTemp, Log, TEXT("RoboQuestGameInstance::No specific loading data found for level: %s"), *LevelName.ToString());
		}

		// 3. Show Widget
		if (!CurrentLoadingWidget->IsInViewport())
		{
			CurrentLoadingWidget->AddToViewport(9999); // Topmost Z-Order
		}
	}

	// 4. Start Timer
	GetWorld()->GetTimerManager().SetTimer(LoadingTimerHandle, this, &URoboQuestGameInstance::UpdateLoadingProgress, 0.1f, true);
}

void URoboQuestGameInstance::UpdateLoadingProgress()
{
	CurrentLoadingTime += 0.1f;

	// float Progress = FMath::Clamp(CurrentLoadingTime / TargetLoadingTime, 0.0f, 1.0f);
	
	// Finish when time is up
	if (CurrentLoadingTime >= TargetLoadingTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(LoadingTimerHandle);

		if (URoboQuestLoadingWidget* LoadingWidget = Cast<URoboQuestLoadingWidget>(CurrentLoadingWidget))
		{
			LoadingWidget->SetLoadingComplete();
		}
		else {
			FinishLoading();
		}
	}
}

void URoboQuestGameInstance::FinishLoading()
{
	UGameplayStatics::OpenLevel(this, PendingLevelName);
	
	// Note: The widget will be destroyed automatically when the level changes
	// unless the widget is added to the GameInstance itself effectively (which AddToViewport on GI created widgets usually persists briefly but clean up is standard with level transition)
    // If you want a fade out AFTER level load, that requires creating the widget again in the new level's BeginPlay.
}