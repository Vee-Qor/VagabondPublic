// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_FinalCut.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/VbCombatComponent.h"
#include "Character/VbPlayerCharacter.h"
#include "Data/VbStyleData.h"
#include "VbGameplayTags.h"
#include "Framework/VbFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"

UVbGA_FinalCut::UVbGA_FinalCut()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_FinalCut.GetTag().GetSingleTagContainer());

    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Attacking);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay_Attack);
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Combat_Unarmed);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Combat_Armed);

    SetInputTag(VbGameplayTags::Input_AbilityOne);
}

void UVbGA_FinalCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbilityCost())
    {
        K2_EndAbility();
        return;
    }
    
    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    UAbilityTask_PlayMontageAndWait* PlayFinalCutMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalCutMontage);
    PlayFinalCutMontage->OnCompleted.AddDynamic(this, &UVbGA_FinalCut::K2_EndAbility);
    PlayFinalCutMontage->OnBlendOut.AddDynamic(this, &UVbGA_FinalCut::K2_EndAbility);
    PlayFinalCutMontage->OnCancelled.AddDynamic(this, &UVbGA_FinalCut::K2_EndAbility);
    PlayFinalCutMontage->OnInterrupted.AddDynamic(this, &UVbGA_FinalCut::K2_EndAbility);
    PlayFinalCutMontage->ReadyForActivation();

    UpdateMotionWarpingTarget();

    if (K2_HasAuthority())
    {
        StartCharge();
    }

    UAbilityTask_WaitGameplayEvent* WaitTraceEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_TraceEnd);
    WaitTraceEndEvent->EventReceived.AddDynamic(this, &UVbGA_FinalCut::OnTraceEndEvent);
    WaitTraceEndEvent->ReadyForActivation();

    UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    WaitInputRelease->OnRelease.AddDynamic(this, &UVbGA_FinalCut::OnInputReleased);
    WaitInputRelease->ReadyForActivation();
}

void UVbGA_FinalCut::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    AttachCamera();
    StopCharge();

    if (K2_HasAuthority())
    {
        SetDamageImmuneEnabled(false);
        BP_RemoveGameplayEffectFromOwnerWithHandle(ChargeAttackPowerEffectClass);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_FinalCut::StartCharge()
{
    IncreaseDamageIndex = 0;

    const UWorld* World = GetWorld();
    if (!World || !DamageMultiplierEffectClass) return;

    World->GetTimerManager().SetTimer(UpdateDamageTimerHandle, this, &UVbGA_FinalCut::UpdateDamage, IncreaseInterval, true);
}

void UVbGA_FinalCut::UpdateDamage()
{
    if (IncreaseDamageIndex >= MaxIncreaseCount)
    {
        StopCharge();
        return;
    }

    if (!K2_CommitAbilityCost())
    {
        return;
    }

    K2_ExecuteGameplayCueWithParams(VbGameplayTags::GameplayCue_FinalCut_Charge, FGameplayCueParameters());

    ChargeAttackPowerEffectClass = BP_ApplyGameplayEffectToOwner(DamageMultiplierEffectClass, 1.0f);

    ++IncreaseDamageIndex;
}

void UVbGA_FinalCut::StopCharge()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(UpdateDamageTimerHandle);
}

void UVbGA_FinalCut::StartFinalCut()
{
    if (!K2_CommitAbilityCooldown())
    {
        K2_EndAbility();
        return;
    }
    UpdateMotionWarpingTarget();

    K2_ExecuteGameplayCueWithParams(VbGameplayTags::GameplayCue_FinalCut, FGameplayCueParameters());
    DetachCamera();

    if (UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance())
    {
        if (CurrentMontage)
        {
            AnimInstance->Montage_JumpToSection(EndSectionName, CurrentMontage);
        }
    }

    if (K2_HasAuthority())
    {
        SetDamageImmuneEnabled(true);

        const AActor* Avatar = GetAvatarActorFromActorInfo();
        if (!Avatar) return;

        AreaTraceInfo.TraceStart = Avatar->GetActorLocation();
        AreaTraceInfo.TraceEnd = Avatar->GetActorLocation();
        AreaTraceInfo.AttackContext.Damage = UVbFunctionLibrary::GetAbilityDamage(GetWorld(), GetClass()).GetValueAtLevel(GetAbilityLevel());
        AreaTraceInfo.AttackContext.ImpactTypeTag = ImpactTypeTag;
        AreaTraceInfo.bFollowOwner = true;
        if (const UVbStyleData* StyleData = GetStyleData())
        {
            AreaTraceInfo.AttackContext.StyleTag = StyleData->GetStyleTag();
        }

        UVbCombatComponent* CombatComponent = GetCombatComponent();
        if (!CombatComponent) return;

        CombatComponent->StartAreaTrace(AreaTraceInfo);
    }
}

void UVbGA_FinalCut::DetachCamera() const
{
    const AVbPlayerCharacter* Character = Cast<AVbPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!Character) return;

    USpringArmComponent* CameraBoom = Character->GetCameraBoom();
    if (!CameraBoom) return;

    CameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void UVbGA_FinalCut::AttachCamera() const
{
    const AVbPlayerCharacter* Character = Cast<AVbPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (!Character) return;

    USpringArmComponent* CameraBoom = Character->GetCameraBoom();
    if (!CameraBoom) return;

    CameraBoom->AttachToComponent(Character->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UVbGA_FinalCut::SetDamageImmuneEnabled(bool bEnabled) const
{
    if (!K2_HasAuthority()) return;

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    if (bEnabled)
    {
        ASC->BP_ApplyGameplayEffectToSelf(DamageImmuneClass, 1.0f, ASC->MakeEffectContext());
        return;
    }

    ASC->RemoveActiveGameplayEffectBySourceEffect(DamageImmuneClass, ASC);
}

void UVbGA_FinalCut::OnTraceEndEvent(FGameplayEventData EventData)
{
    SetDamageImmuneEnabled(false);
}

void UVbGA_FinalCut::OnInputReleased(float TimeHeld)
{
    StopCharge();
    StartFinalCut();
}