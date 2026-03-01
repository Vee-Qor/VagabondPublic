// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/VbGameplayAbility.h"

#include "MotionWarpingComponent.h"
#include "VbGameplayTags.h"
#include "Character/VbCharacter.h"
#include "Character/VbCombatComponent.h"
#include "Data/Enemy/VbEnemyData.h"
#include "Data/Enemy/VbEnemyStyleData.h"
#include "Data/Player/VbPlayerData.h"
#include "Data/Player/VbPlayerStyleData.h"
#include "Framework/VbFunctionLibrary.h"
#include "Weapon/VbWeapon.h"

UVbGameplayAbility::UVbGameplayAbility()
{
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Dying);
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Dead);
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Respawning);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Stun);
    CancelAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay_Defend_Block);
}

void UVbGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
    if (!CooldownEffect) return;

    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffect->GetClass(), GetAbilityLevel(Handle, ActorInfo));
    SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

const FGameplayTagContainer* UVbGameplayAbility::GetCooldownTags() const
{
    FillCooldownInfo();

    FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
    MutableTags->Reset();
    // const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
    // if (ParentTags)
    // {
    //     MutableTags->AppendTags(*ParentTags);
    // }
    MutableTags->AppendTags(CooldownTags);
    return MutableTags;
}

void UVbGameplayAbility::UpdateMotionWarpingTarget() const
{
    const AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    const AController* Controller = Avatar->GetInstigatorController();
    if (!Controller) return;

    const FRotator TargetRotation = Controller->GetControlRotation();

    UMotionWarpingComponent* MotionWarpingComponent = Avatar->FindComponentByClass<UMotionWarpingComponent>();
    if (!MotionWarpingComponent) return;

    FMotionWarpingTarget WarpingTarget;
    //TODO::HARDCODE
    WarpingTarget.Name = "AttackTarget";
    WarpingTarget.Rotation = FRotator(0.0f, TargetRotation.Yaw, 0.0f);

    MotionWarpingComponent->AddOrUpdateWarpTarget(WarpingTarget);
}

AVbCharacter* UVbGameplayAbility::GetVbCharacter() const
{
    return Cast<AVbCharacter>(GetAvatarActorFromActorInfo());
}

UVbCombatComponent* UVbGameplayAbility::GetCombatComponent() const
{
    const AActor* Avatar = GetAvatarActorFromActorInfo();

    return Avatar ? Avatar->FindComponentByClass<UVbCombatComponent>() : nullptr;
}

const UVbCharacterData* UVbGameplayAbility::GetCharacterData() const
{
    const AVbCharacter* Character = GetVbCharacter();

    return Character ? Character->GetCharacterData() : nullptr;
}

const UVbPlayerData* UVbGameplayAbility::GetPlayerData() const
{
    return Cast<UVbPlayerData>(GetCharacterData());
}

const UVbEnemyData* UVbGameplayAbility::GetEnemyData() const
{
    return Cast<UVbEnemyData>(GetCharacterData());
}

const UVbStyleData* UVbGameplayAbility::GetStyleData() const
{
    const UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return nullptr;

    const AVbWeapon* Weapon = CombatComponent->GetCurrentWeapon();

    return Weapon ? Weapon->GetStyleData() : nullptr;
}

const UVbPlayerStyleData* UVbGameplayAbility::GetPlayerStyleData() const
{
    return Cast<UVbPlayerStyleData>(GetStyleData());
}

void UVbGameplayAbility::FillCooldownInfo() const
{
    const FVbAbilityParamsRow* AbilityParamsRow = UVbFunctionLibrary::GetAbilityParamsRow(GetWorld(), GetClass());
    if (!AbilityParamsRow) return;

    CooldownTags.Reset();
    CooldownTags.AddTag(AbilityParamsRow->CooldownTag);
    CooldownDuration = AbilityParamsRow->Cooldown;
}

const UVbEnemyStyleData* UVbGameplayAbility::GetEnemyStyleData() const
{
    return Cast<UVbEnemyStyleData>(GetStyleData());
}