// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Block.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "VbGameplayTags.h"

UVbGA_Block::UVbGA_Block()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Defend_Block.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Blocking);
    ActivationOwnedTags.AddTag(VbGameplayTags::Effect_Blocking);
    ActivationOwnedTags.AddTag(VbGameplayTags::State_HitReact_Immune);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);

    SetInputTag(VbGameplayTags::Input_AbilityTwo);
}

void UVbGA_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    UAbilityTask_PlayMontageAndWait* PlayBlockMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, BlockMontage);
    PlayBlockMontage->OnCompleted.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockMontage->OnBlendOut.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockMontage->ReadyForActivation();

    UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    WaitInputRelease->OnRelease.AddDynamic(this, &UVbGA_Block::OnInputReleased);
    WaitInputRelease->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitBlockedEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Blocked_Hit);
    WaitBlockedEvent->EventReceived.AddDynamic(this, &UVbGA_Block::OnBlockedHit);
    WaitBlockedEvent->ReadyForActivation();

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitParryingStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Parrying_Start);
        WaitParryingStartEvent->EventReceived.AddDynamic(this, &UVbGA_Block::OnParryingStart);
        WaitParryingStartEvent->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitParryingEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Parrying_End);
        WaitParryingEndEvent->EventReceived.AddDynamic(this, &UVbGA_Block::OnParryingEnd);
        WaitParryingEndEvent->ReadyForActivation();
    }
}

void UVbGA_Block::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    RemoveParryingEffect();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_Block::OnBlockedHit(FGameplayEventData EventData)
{
    UAbilityTask_PlayMontageAndWait* PlayHitMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HitMontage);
    PlayHitMontage->OnCompleted.AddDynamic(this, &UVbGA_Block::OnHitMontageFinished);
    PlayHitMontage->OnBlendOut.AddDynamic(this, &UVbGA_Block::OnHitMontageFinished);
    PlayHitMontage->ReadyForActivation();
}

void UVbGA_Block::OnHitMontageFinished()
{
    UAbilityTask_PlayMontageAndWait* PlayLoopMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, BlockMontage, 1.0f,
        LoopSection);
    PlayLoopMontage->OnCompleted.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayLoopMontage->OnBlendOut.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayLoopMontage->ReadyForActivation();
}

void UVbGA_Block::OnInputReleased(float TimeHeld)
{
    UAbilityTask_PlayMontageAndWait* PlayBlockEndSectionMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, BlockMontage,
        1.0f, EndSection);
    PlayBlockEndSectionMontage->OnCompleted.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockEndSectionMontage->OnCancelled.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockEndSectionMontage->OnBlendOut.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockEndSectionMontage->OnInterrupted.AddDynamic(this, &UVbGA_Block::K2_EndAbility);
    PlayBlockEndSectionMontage->ReadyForActivation();
}

void UVbGA_Block::OnParryingStart(FGameplayEventData EventData)
{
    if (ParryingEffectClass)
    {
        ActiveParryingEffectHandle = BP_ApplyGameplayEffectToOwner(ParryingEffectClass);
    }
}

void UVbGA_Block::OnParryingEnd(FGameplayEventData EventData)
{
    RemoveParryingEffect();
}

void UVbGA_Block::RemoveParryingEffect()
{
    if (ActiveParryingEffectHandle.IsValid())
    {
        BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveParryingEffectHandle);
    }
}