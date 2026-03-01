// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Dodge.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Data/Player/VbPlayerStyleData.h"
#include "VbGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

UVbGA_Dodge::UVbGA_Dodge()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Movement_Dodge.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Dodging);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);

    SetInputTag(VbGameplayTags::Input_Dodge);
}

void UVbGA_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    MovementComponent = Avatar->FindComponentByClass<UCharacterMovementComponent>();
    if (!MovementComponent) return;

    UAbilityTask_PlayMontageAndWait* PlayDodgeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, SelectDodgeMontage(),
        1.0f, NAME_None, true);
    PlayDodgeMontage->OnCompleted.AddDynamic(this, &UVbGA_Dodge::K2_EndAbility);
    PlayDodgeMontage->OnCancelled.AddDynamic(this, &UVbGA_Dodge::K2_EndAbility);
    PlayDodgeMontage->OnBlendOut.AddDynamic(this, &UVbGA_Dodge::K2_EndAbility);
    PlayDodgeMontage->OnInterrupted.AddDynamic(this, &UVbGA_Dodge::K2_EndAbility);
    PlayDodgeMontage->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitNonHitStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Evading_Start);
    WaitNonHitStartEvent->EventReceived.AddDynamic(this, &UVbGA_Dodge::OnEvadingStart);
    WaitNonHitStartEvent->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitNonHitEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Evading_End);
    WaitNonHitEndEvent->EventReceived.AddDynamic(this, &UVbGA_Dodge::OnEvadingEnd);
    WaitNonHitEndEvent->ReadyForActivation();
}

void UVbGA_Dodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    RemoveInvulnerability();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAnimMontage* UVbGA_Dodge::SelectDodgeMontage() const
{
    if (!MovementComponent || !Avatar) return nullptr;

    FVbDodgeMontages DodgeMontages = FVbDodgeMontages{};

    if (const UVbPlayerStyleData* PlayerStyleData = GetPlayerStyleData())
    {
        DodgeMontages = PlayerStyleData->GetDodgeMontages();
    }

    FVector DodgeDirection = MovementComponent->GetCurrentAcceleration();

    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = Avatar->GetVelocity();
    }

    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = Avatar->GetActorForwardVector();
    }

    DodgeDirection.Z = 0.0f;
    DodgeDirection = DodgeDirection.GetSafeNormal();

    const FVector LocalDodgeDirection = Avatar->GetActorTransform().InverseTransformVectorNoScale(DodgeDirection);

    UAnimMontage* SelectedDodgeMontage = DodgeMontages.Forward;

    if (LocalDodgeDirection.IsNearlyZero())
    {
        return SelectedDodgeMontage;
    }

    if (FMath::Abs(LocalDodgeDirection.X) > FMath::Abs(LocalDodgeDirection.Y))
    {
        SelectedDodgeMontage = (LocalDodgeDirection.X >= 0.0f) ? DodgeMontages.Forward : DodgeMontages.Backward;
    }
    else
    {
        SelectedDodgeMontage = (LocalDodgeDirection.Y >= 0.0f) ? DodgeMontages.Right : DodgeMontages.Left;
    }

    return SelectedDodgeMontage;
}

void UVbGA_Dodge::OnEvadingStart(FGameplayEventData EventData)
{
    if (!MovementComponent || !Avatar) return;

    FVector DodgeDirection = MovementComponent->GetCurrentAcceleration();

    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = Avatar->GetVelocity();
    }

    if (DodgeDirection.IsNearlyZero())
    {
        DodgeDirection = Avatar->GetActorForwardVector();
    }

    DodgeDirection.Z = 0.0f;
    DodgeDirection = DodgeDirection.GetSafeNormal();

    UAbilityTask_ApplyRootMotionConstantForce* ApplyDodgeRootMotion = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, NAME_None,
        DodgeDirection, DodgeStrength, DodgeDuration, false, nullptr, ERootMotionFinishVelocityMode::ClampVelocity, FVector::ZeroVector, 0.0f, false);
    ApplyDodgeRootMotion->ReadyForActivation();

    ApplyInvulnerability();
}

void UVbGA_Dodge::OnEvadingEnd(FGameplayEventData EventData)
{
    RemoveInvulnerability();
    K2_EndAbility();
}

void UVbGA_Dodge::ApplyInvulnerability()
{
    if (!K2_HasAuthority()) return;

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (ASC && InvulnerabilityEffectClass)
    {
        const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
        const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InvulnerabilityEffectClass, 1.0f, ContextHandle);
        InvulnerabilityEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void UVbGA_Dodge::RemoveInvulnerability()
{
    if (!K2_HasAuthority()) return;

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !InvulnerabilityEffectHandle.IsValid()) return;

    ASC->RemoveActiveGameplayEffect(InvulnerabilityEffectHandle);
}