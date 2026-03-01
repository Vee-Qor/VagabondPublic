// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGAP_HitReact.h"


#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/VbCharacterData.h"
#include "Data/VbStyleData.h"
#include "VbGameplayTags.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"

UVbGAP_HitReact::UVbGAP_HitReact()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Control_HitReact.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_HitReacting);
    ActivationBlockedTags.RemoveTag(VbGameplayTags::Effect_Stun);
    CancelAbilitiesWithTag.RemoveTag(VbGameplayTags::Ability_Gameplay_Defend_Block);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = VbGameplayTags::Event_Ability_HitReact;
    AbilityTriggers.Add(TriggerData);
}

void UVbGAP_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    ExecuteTakenDamageCue(TriggerEventData);

    if (!HasHitReactImmune())
    {
        UAbilityTask_PlayMontageAndWait* PlayDeathMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,
            SelectHitReactMontage(TriggerEventData));
        PlayDeathMontage->OnCompleted.AddDynamic(this, &UVbGAP_HitReact::K2_EndAbility);
        PlayDeathMontage->OnCancelled.AddDynamic(this, &UVbGAP_HitReact::K2_EndAbility);
        PlayDeathMontage->OnBlendOut.AddDynamic(this, &UVbGAP_HitReact::K2_EndAbility);
        PlayDeathMontage->OnInterrupted.AddDynamic(this, &UVbGAP_HitReact::K2_EndAbility);
        PlayDeathMontage->ReadyForActivation();
    }
    else
    {
        K2_EndAbility();
    }
}

void UVbGAP_HitReact::ExecuteTakenDamageCue(const FGameplayEventData* TriggerEventData)
{
    if (!TriggerEventData || !TriggerEventData->ContextHandle.GetHitResult()) return;
    FGameplayCueParameters CueParams;
    CueParams.AggregatedSourceTags = TriggerEventData->InstigatorTags;
    CueParams.EffectContext = TriggerEventData->ContextHandle;
    CueParams.Location = TriggerEventData->ContextHandle.GetHitResult()->ImpactPoint;
    CueParams.Normal = TriggerEventData->ContextHandle.GetHitResult()->ImpactNormal;
    if (TriggerEventData->ContextHandle.GetHitResult())
    {
        CueParams.PhysicalMaterial = TriggerEventData->ContextHandle.GetHitResult()->PhysMaterial;
    }

    ReportSenseEvents(TriggerEventData);
    K2_ExecuteGameplayCueWithParams(VbGameplayTags::GameplayCue_TakenDamage, CueParams);
}

UAnimMontage* UVbGAP_HitReact::SelectHitReactMontage(const FGameplayEventData* TriggerEventData) const
{
    const FVbHitReactMontages& HitReactMontages = GetHitReactMontages();

    if (!TriggerEventData) return HitReactMontages.Front;

    const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult();
    if (!HitResult) return HitReactMontages.Front;

    const AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return HitReactMontages.Front;

    const FVector ForwardVector = Avatar->GetActorForwardVector();
    const FVector RightVector = Avatar->GetActorRightVector();
    const FVector HitNormal = HitResult->ImpactNormal;

    const float ForwardDot = FVector::DotProduct(ForwardVector, HitNormal);
    const float RightDot = FVector::DotProduct(RightVector, HitNormal);

    if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
    {
        if (ForwardDot > 0.0f)
        {
            return HitReactMontages.Front;
        }

        return HitReactMontages.Back;
    }

    if (RightDot > 0.0f)
    {
        return HitReactMontages.Right;
    }

    return HitReactMontages.Left;
}

FVbHitReactMontages UVbGAP_HitReact::GetHitReactMontages() const
{
    if (const UVbCharacterData* CharacterData = GetCharacterData())
    {
        return CharacterData->GetHitReactMontages();
    }

    return FVbHitReactMontages();
}

bool UVbGAP_HitReact::HasHitReactImmune() const
{
    const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    return ASC ? ASC->HasMatchingGameplayTag(VbGameplayTags::State_HitReact_Immune) : false;
}

void UVbGAP_HitReact::ReportSenseEvents(const FGameplayEventData* TriggerEventData)
{
    if (!TriggerEventData) return;

    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult();
    if (!HitResult) return;

    AActor* Instigator = TriggerEventData->ContextHandle.GetInstigator();
    if (!Instigator) return;

    const float Damage = TriggerEventData->EventMagnitude;

    UAISense_Damage::ReportDamageEvent(this, Avatar, Instigator, Damage, Instigator->GetActorLocation(), HitResult->ImpactPoint);
    UAISense_Hearing::ReportNoiseEvent(this, HitResult->ImpactPoint, 1.0f, Instigator);
}