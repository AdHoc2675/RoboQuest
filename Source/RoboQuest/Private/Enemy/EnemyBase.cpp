// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusComponent.h"
#include "RoboQuest/RoboQuestCharacter.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// bind to health changed event
    if (StatusComponent)
    {
        StatusComponent->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnHealthChanged);
    }
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// apply damage to status component
    if (StatusComponent && IsAlive())
    {
        StatusComponent->TakeDamage(ActualDamage);
        
		// If there is an aggro system, set the DamageCauser as the target here

        // can add additional reactions to health changes here (e.g., play hurt animations, sounds, etc.)
    }

    return ActualDamage;
}

void AEnemyBase::OnHealthChanged(float CurrentHealth, float ScratchHealth, float MaxHealth)
{
	// Check for death
    if (CurrentHealth <= 0.0f && IsAlive())
    {
        Die();
    }

}

void AEnemyBase::Die()
{
    if (bIsDead) return;
    
    bIsDead = true;

	// give exp to player
    ARoboQuestCharacter* PlayerCharacter = Cast<ARoboQuestCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (PlayerCharacter && StatusComponent)
    {
        PlayerCharacter->GetStatusComponent()->AddExp(StatusComponent->ExpReward);
	}

	// Disable collisions
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); // È¤Àº NoCollision

	// enable ragdoll physics
    GetMesh()->SetSimulatePhysics(true);

    // Detach controller
    DetachFromControllerPendingDestroy();

    // Destroy actor after a delay (set LifeSpan)
    SetLifeSpan(5.0f); 
}


