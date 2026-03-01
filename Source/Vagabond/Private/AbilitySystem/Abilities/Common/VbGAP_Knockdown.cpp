// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGAP_Knockdown.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "VbGameplayTags.h"

UVbGAP_Knockdown::UVbGAP_Knockdown()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Control_Knockdown.GetTag().GetSingleTagContainer());
    CancelAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = VbGameplayTags::Event_Ability_Knockdown;
    AbilityTriggers.Add(TriggerData);
}

void UVbGAP_Knockdown::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }
    if (K2_HasAuthority() && KnockdownEffectClass)
    {
        UAbilityTask_WaitGameplayEvent* WaitKnockdownEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Knockdown_End);
        WaitKnockdownEndEvent->EventReceived.AddDynamic(this, &UVbGAP_Knockdown::OnEndKnockdown);
        WaitKnockdownEndEvent->ReadyForActivation();

        ActiveKnockdownEffectHandle = ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo,
            MakeOutgoingGameplayEffectSpec(KnockdownEffectClass, 1.0f));
    }
}

void UVbGAP_Knockdown::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    RemoveKnockdownEffect();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGAP_Knockdown::RemoveKnockdownEffect()
{
    if (ActiveKnockdownEffectHandle.IsValid())
    {
        BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveKnockdownEffectHandle);
    }
}

void UVbGAP_Knockdown::OnEndKnockdown(FGameplayEventData EventData)
{
    RemoveKnockdownEffect();

    K2_EndAbility();
}