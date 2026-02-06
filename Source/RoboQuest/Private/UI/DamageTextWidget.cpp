// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/PlayerController.h"

void UDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDamageTextWidget::PlayDamageText(float Damage, FVector InWorldLocation, bool bIsCritical)
{
	// 1. Set Value (Visual Setup)
	if (DamageText)
	{
		// Display as integer by removing decimals
		int32 DamageInt = FMath::RoundToInt(Damage);
		DamageText->SetText(FText::AsNumber(DamageInt));

		// Set Color & Initial Scale
		if (bIsCritical)
		{
			DamageText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f))); // Red
			StartScale = FVector2D(1.5f, 1.5f);
		}
		else
		{
			DamageText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))); // White
			StartScale = FVector2D(1.0f, 1.0f);
		}

		TargetScale = StartScale * 0.6f;
		SetRenderScale(StartScale);
	}

	// 2. Setup Location
	WorldLocation = InWorldLocation;
	CurrentLifeTime = 0.0f;

	// 3. Calculate Distance Scale
	//    Ideally, we want visual movement on screen to be consistent regardless of distance.
	//    To achieve this, World Velocity must be proportional to Distance.
	float DistScale = 1.0f;
	if (APlayerController* PC = GetOwningPlayer())
	{
		FVector CameraLoc = PC->PlayerCameraManager ? PC->PlayerCameraManager->GetCameraLocation() : PC->GetPawn()->GetActorLocation();
		float Distance = FVector::Dist(CameraLoc, InWorldLocation);

		// Scale = Distance / Reference (e.g., if Dist is 2000 and Ref is 1000, Scale is 2.0)
		DistScale = Distance / (ReferenceDistance > 0.1f ? ReferenceDistance : 1000.0f);
		DistScale = FMath::Clamp(DistScale, MinScale, MaxScale);
	}

	// 4. Initialize Physics with Scaling
	FVector RandomDir = FMath::VRand();
	RandomDir.Z = 0.0f; 
	RandomDir.Normalize();
	
	// Apply Scale to Speeds
	float ScaledHorizontal = HorizontalSpeed * DistScale;
	float ScaledVertical = InitialVerticalSpeed * DistScale;
	
	// Apply Scale to Gravity as well 
	// (Scaling Gravity ensures the arc shape remains the same visually)
	AppliedGravity = Gravity * DistScale; 

	// Set Velocity
	Velocity = RandomDir * FMath::RandRange(ScaledHorizontal * 0.8f, ScaledHorizontal * 1.2f);
	Velocity.Z = ScaledVertical;

	// Add to Viewport
	if (!IsInViewport())
	{
		AddToViewport();
	}
}

void UDamageTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CurrentLifeTime += InDeltaTime;

	if (CurrentLifeTime >= Duration)
	{
		RemoveFromParent();
		return;
	}

	// --- 1. Physics Update (Using AppliedGravity) ---
	Velocity.Z -= AppliedGravity * InDeltaTime;
	WorldLocation += Velocity * InDeltaTime;


	// --- 2. Scale Animation (Shrink) ---
	float LifeAlpha = CurrentLifeTime / Duration;
	FVector2D NewScale = FMath::Lerp(StartScale, TargetScale, LifeAlpha);
	SetRenderScale(NewScale);


	// --- 3. Screen Projection ---
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	FVector2D ScreenPosition;
	bool bProjected = UGameplayStatics::ProjectWorldToScreen(PC, WorldLocation, ScreenPosition);

	if (bProjected)
	{
		SetPositionInViewport(ScreenPosition);
		
		// Fade out logic
		float FadeStart = Duration * 0.5f;
		if (CurrentLifeTime > FadeStart)
		{
			float Alpha = 1.0f - ((CurrentLifeTime - FadeStart) / (Duration - FadeStart));
			SetRenderOpacity(Alpha);
		}
		else
		{
			SetRenderOpacity(1.0f);
		}
	}
}

