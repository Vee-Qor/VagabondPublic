// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_MillionStrikes.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "VbGameplayTags.h"

UVbGA_MillionStrikes::UVbGA_MillionStrikes()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_MillionStrikes.GetTag().GetSingleTagContainer());

    SetInputTag(VbGameplayTags::Input_AbilityOne);
}

void UVbGA_MillionStrikes::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance())
    {
        if (CurrentMontage)
        {
            AnimInstance->Montage_SetNextSection(LoopSectionName, EndSectionName, CurrentMontage);
        }
    }

    if (const UWorld* World = GetWorld())
    {
        if (LoopCheckTimer.IsValid())
        {
            World->GetTimerManager().ClearTimer(LoopCheckTimer);
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_MillionStrikes::ExecuteAttack()
{
    Super::ExecuteAttack();

    K2_AddGameplayCueWithParams(VbGameplayTags::GameplayCue_MillionStrikes, FGameplayCueParameters());

    if (const UWorld* World = GetWorld())
    {
        bExitRequested = false;
        bMaxSpeedReached = false;
        ValidClickCount = 0;

        if (const UAnimInstance* Anim = GetActorInfo().GetAnimInstance())
        {
            if (CurrentMontage)
            {
                BaseAttackSpeed = Anim->Montage_GetPlayRate(CurrentMontage);
            }
        }

        LoopStartTime = World->GetTimeSeconds();
        LoopEndTime = LoopStartTime + MinLoopDuration;
        NextAllowedClickTime = LoopStartTime;

        World->GetTimerManager().SetTimer(LoopCheckTimer, this, &UVbGA_MillionStrikes::CheckLoopExit, 0.05f, true);
    }

    UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
    WaitInputPress->OnPress.AddDynamic(this, &UVbGA_MillionStrikes::OnInputPressed);
    WaitInputPress->ReadyForActivation();
}

void UVbGA_MillionStrikes::OnInputPressed(float TimeWaited)
{
    UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
    WaitInputPress->OnPress.AddDynamic(this, &UVbGA_MillionStrikes::OnInputPressed);
    WaitInputPress->ReadyForActivation();

    if (bExitRequested) return;

    const UWorld* World = GetWorld();
    if (!World || World->GetTimeSeconds() < NextAllowedClickTime) return;

    TryProcessClick();
}

void UVbGA_MillionStrikes::CheckLoopExit()
{
    if (bExitRequested) return;

    if (const UWorld* World = GetWorld())
    {
        if (World->GetTimeSeconds() >= LoopEndTime)
        {
            RequestExitLoop();
            bExitRequested = true;
            World->GetTimerManager().ClearTimer(LoopCheckTimer);
        }
    }
}

void UVbGA_MillionStrikes::RequestExitLoop() const
{
    if (UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance())
    {
        if (CurrentMontage)
        {
            AnimInstance->Montage_SetNextSection(LoopSectionName, EndSectionName, CurrentMontage);
        }
    }
}

void UVbGA_MillionStrikes::TryProcessClick()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    const float Now = World->GetTimeSeconds();

    if (bMaxSpeedReached) return;

    if (!K2_CommitAbilityCost())
    {
        return;
    }

    NextAllowedClickTime = Now + ClickCooldown;

    ++ValidClickCount;

    LoopEndTime += LoopExtendPerClick;

    const int32 SafeClicksToMax = FMath::Max(1, ClicksToMax);
    const int32 ClampedClicks = FMath::Clamp(ValidClickCount, 0, SafeClicksToMax);
    const float SpeedStep = (MaxAttackSpeed - BaseAttackSpeed) / SafeClicksToMax;
    const float NewRate = FMath::Clamp(BaseAttackSpeed + SpeedStep * ClampedClicks, BaseAttackSpeed, MaxAttackSpeed);

    if (UAnimInstance* Anim = GetActorInfo().GetAnimInstance())
    {
        if (CurrentMontage)
        {
            Anim->Montage_SetPlayRate(CurrentMontage, NewRate);
        }
    }

    const float BonusTimePerClick = BonusTimeAtMaxSpeed / SafeClicksToMax;
    const float ScaledBonusTime = BonusTimePerClick * ClampedClicks;

    LoopEndTime = FMath::Max(LoopEndTime, Now + ScaledBonusTime);

    if (ClampedClicks >= SafeClicksToMax)
    {
        bMaxSpeedReached = true;
    }
}