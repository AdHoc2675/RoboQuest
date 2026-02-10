// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Ability_ShotgunBlast.h"
#include "../RoboQuestProjectile.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

void UAbility_ShotgunBlast::OnActivate_Implementation()
{
	if (!CharacterOwner || !ProjectileClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// Muzzle location could also be used here if available
	FVector StartLoc;
	FRotator BaseRot;
	CharacterOwner->GetActorEyesViewPoint(StartLoc, BaseRot);
	
	// Adjust spawn location slightly forward to avoid collision with the character
	FVector SpawnLoc = StartLoc + (BaseRot.Vector() * 50.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = CharacterOwner;
	SpawnParams.Instigator = CharacterOwner;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int32 i = 0; i < PelletCount; i++)
	{
		// Calculate random spread within a cone
		FRotator RandomRot = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(BaseRot.Vector(), SpreadAngle).Rotation();

		ARoboQuestProjectile* Proj = World->SpawnActor<ARoboQuestProjectile>(ProjectileClass, SpawnLoc, RandomRot, SpawnParams);
		if (Proj)
		{
			Proj->Tags.Add(FName("Ability_Shotgun"));
			Proj->CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			Proj->InitializeProjectile(DamagePerPellet, 15.0f /*Range*/, 1.5f /*Crit*/);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Ability_ShotgunBlast::::Shotgun Blast Activated: Fired %d pellets."), PelletCount);
    
	// Optionally, play firing sound here (e.g., UGameplayStatics::PlaySoundAtLocation...)
}

