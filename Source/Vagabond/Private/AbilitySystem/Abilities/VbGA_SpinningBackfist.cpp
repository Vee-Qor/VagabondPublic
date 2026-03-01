// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/VbGA_SpinningBackfist.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "VbGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UVbGA_SpinningBackfist::UVbGA_SpinningBackfist()
{
    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_SpinningBackfist.GetTag().GetSingleTagContainer());
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);

    SetInputTag(VbGameplayTags::Input_AbilityTwo);
}

void UVbGA_SpinningBackfist::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
    {
        OwnerASC->RemoveActiveGameplayEffectBySourceEffect(DamageImmuneClass, OwnerASC);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_SpinningBackfist::ExecuteAttack()
{
    Super::ExecuteAttack();

    ensure(StunEffectClass);

    UVbAbilitySystemComponent* OwnerASC = Cast<UVbAbilitySystemComponent>(
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()));
    if (!OwnerASC) return;

    OwnerASC->ApplyEffect(DamageImmuneClass);

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitStunEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Stun);
        WaitStunEvent->EventReceived.AddDynamic(this, &UVbGA_SpinningBackfist::OnStunEventReceived);
        WaitStunEvent->ReadyForActivation();
    }
}

FVbAttackContext UVbGA_SpinningBackfist::BuildAttackContext(const UVbAttackTracePayload* Payload)
{
    FVbAttackContext AttackContext = Super::BuildAttackContext(Payload);
    AttackContext.OnHitEventTag = VbGameplayTags::Event_Stun;
    return AttackContext;
}

void UVbGA_SpinningBackfist::OnStunEventReceived(FGameplayEventData EventData)
{
    AActor* TargetActor = const_cast<AActor*>(EventData.Target.Get());
    if (!TargetActor || !StunEffectClass) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (!TargetASC || !SourceASC) return;

    FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

    const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(StunEffectClass, 1.0f, ContextHandle);
    SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}