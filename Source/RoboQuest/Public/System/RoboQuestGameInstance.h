// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RoboQuestGameInstance.generated.h"

// Struct to hold data for specific level loading screens
USTRUCT(BlueprintType)
struct FLevelLoadingData
{
	GENERATED_BODY()

	// The background image for this level
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BackgroundImage;

	// The display name of the level (e.g., "Canyon")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LevelDisplayName;

	// Tip title (e.g., "Wrench")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TipTitle;

	// Tip description text
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TipDescription;
};

/**
 * Manages game state and level transitions with a fake loading screen.
 */
UCLASS()
class ROBOQUEST_API URoboQuestGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Reference to the Widget Blueprint class (WBP_LoadingScreen)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class URoboQuestLoadingWidget> LoadingWidgetClass;

	// Map to configure data per Level Name (Key: LevelName, Value: Data)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TMap<FName, FLevelLoadingData> LevelLoadingDataMap;

	// Function to transition levels with the visual loading screen
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void OpenLevelWithFakeLoading(FName LevelName, float FakeDuration = 10.0f);

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void FinishLoading(); // Public으로 변경 (위젯에서 호출)

protected:
	void UpdateLoadingProgress();

private:
	UPROPERTY()
	class URoboQuestLoadingWidget* CurrentLoadingWidget;

	FName PendingLevelName;
	FTimerHandle LoadingTimerHandle;
	float CurrentLoadingTime;
	float TargetLoadingTime;
};