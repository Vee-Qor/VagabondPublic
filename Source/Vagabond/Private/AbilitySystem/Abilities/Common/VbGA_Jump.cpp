// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Jump.h"

#include "Character/VbCharacter.h"
#include "VbGameplayTags.h"
#include "GameFramework/Character.h"

UVbGA_Jump::UVbGA_Jump()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Movement_Jump.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Jumping);
    ActivationBlockedTags.AddTag(VbGameplayTags::Action);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);

    SetInputTag(VbGameplayTags::Input_Jump);
}

bool UVbGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }
    
    if (const AVbCharacter* OwnerCharacter = GetVbCharacter())
    {
        return OwnerCharacter->CanJump();
    }

    return false;
}

void UVbGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    if (AVbCharacter* OwnerCharacter = GetVbCharacter())
    {
        OwnerCharacter->Jump();
        OwnerCharacter->LandedDelegate.AddDynamic(this, &UVbGA_Jump::OnLanded);
    }
}

void UVbGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (AVbCharacter* OwnerCharacter = GetVbCharacter())
    {
        OwnerCharacter->LandedDelegate.RemoveAll(this);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_Jump::OnLanded(const FHitResult& HitResult)
{
    K2_EndAbility();
}