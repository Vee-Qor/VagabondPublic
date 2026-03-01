// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_SwitchWeapon.h"

#include "Character/VbCombatComponent.h"
#include "VbGameplayTags.h"

UVbGA_SwitchWeapon::UVbGA_SwitchWeapon()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    ActivationBlockedTags.AddTag(VbGameplayTags::Action_Weapon);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);

    SetInputTag(VbGameplayTags::Input_Weapon_Switch);
}

void UVbGA_SwitchWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;

    if (UVbCombatComponent* CombatComponent = GetCombatComponent())
    {
        CombatComponent->SwitchWeapon();
    }

    K2_EndAbility();
}