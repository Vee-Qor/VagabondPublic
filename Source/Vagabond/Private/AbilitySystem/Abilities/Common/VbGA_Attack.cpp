// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "VbGameplayTags.h"

UVbGA_Attack::UVbGA_Attack()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Attacking);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay_Attack);
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Combat_Unarmed);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Combat_Armed);

    SetInputTag(VbGameplayTags::Input_Attack_Primary);
}

void UVbGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ensure(!AttackMontages.IsEmpty());

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    if (ResetComboTimerHandle.IsValid())
    {
        const UWorld* World = GetWorld();
        if (!World) return;

        World->GetTimerManager().ClearTimer(ResetComboTimerHandle);
    }

    if (ComboIndex >= AttackMontages.Num())
    {
        ResetCombo();
    }

    UpdateMotionWarpingTarget();

    UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,
        AttackMontages[ComboIndex], 1.0f, NAME_None, false);
    PlayAttackMontage->OnCompleted.AddDynamic(this, &UVbGA_Attack::K2_EndAbility);
    PlayAttackMontage->OnCancelled.AddDynamic(this, &UVbGA_Attack::K2_EndAbility);
    PlayAttackMontage->OnBlendOut.AddDynamic(this, &UVbGA_Attack::K2_EndAbility);
    PlayAttackMontage->OnInterrupted.AddDynamic(this, &UVbGA_Attack::K2_EndAbility);
    PlayAttackMontage->ReadyForActivation();

    ExecuteAttack();

    UAbilityTask_WaitGameplayEvent* WaitAttackEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_End);
    WaitAttackEndEvent->EventReceived.AddDynamic(this, &UVbGA_Attack::OnAttackEnd);
    WaitAttackEndEvent->ReadyForActivation();

    ComboIndex++;
}

void UVbGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(ResetComboTimerHandle, this, &UVbGA_Attack::ResetCombo, ComboResetDelay);
    }
}

void UVbGA_Attack::ExecuteAttack()
{
}

void UVbGA_Attack::ResetCombo()
{
    ComboIndex = 0;
}

void UVbGA_Attack::OnAttackEnd(FGameplayEventData EventData)
{
    K2_EndAbility();
}