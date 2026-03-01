// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGAP_Respawn.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/VbCharacterData.h"
#include "Framework/VbGameMode.h"
#include "VbGameplayTags.h"
#include "Kismet/GameplayStatics.h"

UVbGAP_Respawn::UVbGAP_Respawn()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_System_Respawn.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::State_Respawning);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Dead);
    CancelAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);

    ActivationBlockedTags.RemoveTag(VbGameplayTags::State_Dead);
    ActivationBlockedTags.RemoveTag(VbGameplayTags::Effect_Stun);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = VbGameplayTags::Event_Ability_Respawn;
    AbilityTriggers.Add(TriggerData);
}

void UVbGAP_Respawn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    UAbilityTask_PlayMontageAndWait* PlayRespawnMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,
        GetRespawnMontage(TriggerEventData), 1.0f, NAME_None, false);
    PlayRespawnMontage->OnCompleted.AddDynamic(this, &UVbGAP_Respawn::K2_EndAbility);
    PlayRespawnMontage->OnCancelled.AddDynamic(this, &UVbGAP_Respawn::K2_EndAbility);
    PlayRespawnMontage->OnBlendOut.AddDynamic(this, &UVbGAP_Respawn::K2_EndAbility);
    PlayRespawnMontage->OnInterrupted.AddDynamic(this, &UVbGAP_Respawn::K2_EndAbility);
    PlayRespawnMontage->ReadyForActivation();

    if (K2_HasAuthority())
    {
        Respawn();
    }

    UAbilityTask_WaitGameplayEvent* WaitRespawnEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Respawn_End);
    WaitRespawnEndEvent->EventReceived.AddDynamic(this, &UVbGAP_Respawn::EndRespawning);
    WaitRespawnEndEvent->ReadyForActivation();
}

UAnimMontage* UVbGAP_Respawn::GetRespawnMontage(const FGameplayEventData* TriggerEventData) const
{
    if (!TriggerEventData) return nullptr;

    const UVbCharacterData* CharacterData = GetCharacterData();
    if (!CharacterData) return nullptr;

    const TArray<UAnimMontage*>& RespawnMontages = CharacterData->GetRespawnMontages();
    if (RespawnMontages.IsEmpty()) return nullptr;

    return RespawnMontages[TriggerEventData->EventMagnitude];
}

void UVbGAP_Respawn::Respawn() const
{
    if (!RespawnEffect) return;

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    const AVbGameMode* GameMode = Cast<AVbGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GameMode) return;

    const TSubclassOf<UGameplayEffect> DeadEffectClass = GameMode->GetDeadEffectClass();
    if (!DeadEffectClass) return;

    ASC->RemoveActiveGameplayEffectBySourceEffect(DeadEffectClass, ASC);
    ASC->BP_ApplyGameplayEffectToSelf(RespawnEffect, 1.0f, ASC->MakeEffectContext());
}

void UVbGAP_Respawn::EndRespawning(FGameplayEventData EventData)
{
    K2_EndAbility();
}