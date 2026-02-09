// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/Ability_Frenzy.h"
#include "RoboQuest/RoboQuestCharacter.h"
#include "RoboQuest/TP_WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StatusComponent.h"
#include "TimerManager.h"

UAbility_Frenzy::UAbility_Frenzy()
{
    AbilityName = FText::FromString(TEXT("Frenzy"));
    CooldownDuration = 0.0f; // Passive, no cooldown
}

void UAbility_Frenzy::InitializeAbility(ARoboQuestCharacter* Owner)
{
    Super::InitializeAbility(Owner);
}

void UAbility_Frenzy::OnPassiveActivate_Implementation()
{
    if (CharacterOwner)
    {
        // Bind to Character events
        CharacterOwner->OnEnemyKilled.AddDynamic(this, &UAbility_Frenzy::HandleEnemyKilled);
        CharacterOwner->OnBossHitByAbility.AddDynamic(this, &UAbility_Frenzy::HandleBossHit);

        // Start the update loop for decay and stats application
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UAbility_Frenzy::UpdateFrenzyLoop, UpdateInterval, true);
        }

        UE_LOG(LogTemp, Log, TEXT("Frenzy Passive Activated"));
    }
}

void UAbility_Frenzy::HandleEnemyKilled()
{
    AddStacks(StacksPerKill);
}

void UAbility_Frenzy::HandleBossHit()
{
    AddStacks(StacksPerBossHit);
}

void UAbility_Frenzy::AddStacks(float Amount)
{
    CurrentStacks = FMath::Clamp(CurrentStacks + Amount, 0.0f, MaxStacks);

    // Immediately apply buffs to feel responsive
    ApplyBuffs();
}

void UAbility_Frenzy::UpdateFrenzyLoop()
{
    if (CurrentStacks > 0.0f)
    {
        // Calculate decay for this interval
        float DecayAmount = DecayRate * UpdateInterval;
        CurrentStacks = FMath::Max(0.0f, CurrentStacks - DecayAmount);

        ApplyBuffs();
    }
}

void UAbility_Frenzy::ApplyBuffs()
{
    if (!CharacterOwner) return;

    // Use current stack count for multiplier
    float EffectMultiplier = CurrentStacks;

    if (UTP_WeaponComponent* Weapon = CharacterOwner->FindComponentByClass<UTP_WeaponComponent>())
    {
        // Calculate Buff Values based on Stacks
        // FireRate: +1% per stack (e.g., 10 stacks = 0.1)
        float NewFireRateBonus = EffectMultiplier * 0.01f;
        
        // ReloadSpeed: +1% per stack
        float NewReloadBonus = EffectMultiplier * 0.01f;

        // Movement Speed: +0.5% per stack
        float NewMoveSpeedBonus = EffectMultiplier * 0.005f;

        Weapon->FrenzyFireRateMod = NewFireRateBonus;
        Weapon->FrenzyReloadSpeedMod = NewReloadBonus;
        Weapon->FrenzyMoveSpeedMod = NewMoveSpeedBonus; 

        // Trigger recalculation to update 'Final' stats
        Weapon->RecalculateStats();
    }
}