// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_EquipWeapon.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/VbCombatComponent.h"
#include "Data/Player/VbPlayerStyleData.h"
#include "VbGameplayTags.h"

UVbGA_EquipWeapon::UVbGA_EquipWeapon()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Weapon_Equip.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Weapon_Equipping);
    ActivationBlockedTags.AddTag(VbGameplayTags::Action);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Combat_Unarmed);

    SetInputTag(VbGameplayTags::Input_Weapon_Equip);
}

void UVbGA_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

    UAbilityTask_PlayMontageAndWait* PlayEquipMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GetEquipMontage());
    PlayEquipMontage->OnCompleted.AddDynamic(this, &UVbGA_EquipWeapon::K2_EndAbility);
    PlayEquipMontage->OnCancelled.AddDynamic(this, &UVbGA_EquipWeapon::K2_EndAbility);
    PlayEquipMontage->OnBlendOut.AddDynamic(this, &UVbGA_EquipWeapon::K2_EndAbility);
    PlayEquipMontage->OnInterrupted.AddDynamic(this, &UVbGA_EquipWeapon::K2_EndAbility);
    PlayEquipMontage->ReadyForActivation();

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitEquipWeaponEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Weapon_Equip);
        WaitEquipWeaponEvent->EventReceived.AddDynamic(this, &UVbGA_EquipWeapon::EquipWeaponEvent);
        WaitEquipWeaponEvent->ReadyForActivation();
    }
}

UAnimMontage* UVbGA_EquipWeapon::GetEquipMontage() const
{
    const UVbPlayerStyleData* PlayerStyleData = GetPlayerStyleData();

    return PlayerStyleData ? PlayerStyleData->GetEquipMontage() : nullptr;
}

void UVbGA_EquipWeapon::EquipWeaponEvent(FGameplayEventData EventData)
{
    const UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return;

    CombatComponent->SetArmedState(true);
}