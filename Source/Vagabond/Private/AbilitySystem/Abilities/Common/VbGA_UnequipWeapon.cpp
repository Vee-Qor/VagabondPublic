// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_UnequipWeapon.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/VbCombatComponent.h"
#include "Data/Player/VbPlayerStyleData.h"
#include "VbGameplayTags.h"

UVbGA_UnequipWeapon::UVbGA_UnequipWeapon()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Weapon_Unequip.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Weapon_Unequipping);
    ActivationBlockedTags.AddTag(VbGameplayTags::Action);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Combat_Armed);

    SetInputTag(VbGameplayTags::Input_Weapon_Unequip);
}

void UVbGA_UnequipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    UAbilityTask_PlayMontageAndWait* PlayUnequipMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetUnequipMontage());
    PlayUnequipMontage->OnCompleted.AddDynamic(this, &UVbGA_UnequipWeapon::K2_EndAbility);
    PlayUnequipMontage->OnCancelled.AddDynamic(this, &UVbGA_UnequipWeapon::K2_EndAbility);
    PlayUnequipMontage->OnBlendOut.AddDynamic(this, &UVbGA_UnequipWeapon::K2_EndAbility);
    PlayUnequipMontage->OnInterrupted.AddDynamic(this, &UVbGA_UnequipWeapon::K2_EndAbility);
    PlayUnequipMontage->ReadyForActivation();

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitUnequipWeaponEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Weapon_Unequip);
        WaitUnequipWeaponEvent->EventReceived.AddDynamic(this, &UVbGA_UnequipWeapon::UnequipWeaponEvent);
        WaitUnequipWeaponEvent->ReadyForActivation();
    }
}

UAnimMontage* UVbGA_UnequipWeapon::GetUnequipMontage() const
{
    const UVbPlayerStyleData* PlayerStyleData = GetPlayerStyleData();

    return PlayerStyleData ? PlayerStyleData->GetUnequipMontage() : nullptr;
}

void UVbGA_UnequipWeapon::UnequipWeaponEvent(FGameplayEventData EventData)
{
    const UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return;

    CombatComponent->SetArmedState(false);
}