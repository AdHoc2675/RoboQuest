// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Ability_HomingMissile.h"
#include "RoboQuestHomingProjectile.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyBotBase.h"

void UAbility_HomingMissile::OnActivate_Implementation()
{
	if (!CharacterOwner || !ProjectileClass) return;

	USceneComponent* TargetComp = FindNearestEnemy();
	
	// Even if there is no target, we will fire (straight). If there is a target, it will be guided.
	if (UWorld* World = GetWorld())
	{
		// Spawn from character location + offset in facing direction
		FVector SpawnLocation = CharacterOwner->GetActorLocation() + (CharacterOwner->GetActorRotation().RotateVector(SpawnOffset));
		FRotator SpawnRotation = CharacterOwner->GetControlRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = CharacterOwner;
		SpawnParams.Instigator = CharacterOwner;

		ARoboQuestHomingProjectile* Missile = World->SpawnActor<ARoboQuestHomingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Missile)
		{
			Missile->Tags.Add(FName("Ability_Missile"));
			Missile->InitializeProjectile(90.0f, 50.0f, 1.5f); // set damage etc. (recommend to make variables)
			
			if (TargetComp)
			{
				Missile->SetHomingTarget(TargetComp);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Ability_HomingMissile::::Homing Missile Activated: Target %s"), TargetComp ? *TargetComp->GetName() : TEXT("None"));
}

USceneComponent* UAbility_HomingMissile::FindNearestEnemy()
{
	if (!CharacterOwner) return nullptr;

	TArray<AActor*> FoundEnemies;
	// Searching for all EnemyBotBase class (modify according to the enemy base class)
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBotBase::StaticClass(), FoundEnemies);

	AActor* NearestEnemy = nullptr;
	float MinDistanceSq = SearchRadius * SearchRadius;
	FVector PlayerLoc = CharacterOwner->GetActorLocation();

	for (AActor* Enemy : FoundEnemies)
	{
		// may need to check if it's alive (interface or function needed)
		if (Enemy)
		{
			float DistSq = FVector::DistSquared(PlayerLoc, Enemy->GetActorLocation());
			if (DistSq < MinDistanceSq)
			{
				MinDistanceSq = DistSq;
				NearestEnemy = Enemy;
			}
		}
	}

	return NearestEnemy ? NearestEnemy->GetRootComponent() : nullptr;
}

