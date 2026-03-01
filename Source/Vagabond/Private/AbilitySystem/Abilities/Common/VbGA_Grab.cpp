// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Grab.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/VbCharacter.h"
#include "VbGameplayTags.h"
#include "GameFramework/Character.h"

UVbGA_Grab::UVbGA_Grab()
{
    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_Grab.GetTag().GetSingleTagContainer());
}

void UVbGA_Grab::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (K2_HasAuthority())
    {
        if (TargetASC && ActiveGrabbedEffectHandle.IsValid())
        {
            TargetASC->RemoveActiveGameplayEffect(ActiveGrabbedEffectHandle);
        }
    }

    TargetCharacter = nullptr;
    TargetASC = nullptr;
    ActiveGrabbedEffectHandle = FActiveGameplayEffectHandle();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_Grab::ExecuteAttack()
{
    Super::ExecuteAttack();

    UAbilityTask_WaitGameplayEvent* WaitGrabStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Grab_Start);
    WaitGrabStartEvent->EventReceived.AddDynamic(this, &UVbGA_Grab::StartGrab);
    WaitGrabStartEvent->ReadyForActivation();
}

FVbAttackContext UVbGA_Grab::BuildAttackContext(const UVbAttackTracePayload* Payload)
{
    FVbAttackContext AttackContext = Super::BuildAttackContext(Payload);
    AttackContext.OnHitEventTag = VbGameplayTags::Event_Grab_Start;
    return AttackContext;
}

void UVbGA_Grab::StartGrab(FGameplayEventData EventData)
{
    TargetCharacter = Cast<AVbCharacter>(const_cast<AActor*>(EventData.Target.Get()));
    if (!TargetCharacter || !GrabbedEffectClass) return;

    TargetCharacter->SetGrabContext(GetAvatarActorFromActorInfo(), GrabSocket);

    if (K2_HasAuthority())
    {
        const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
        TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
        if (!TargetASC || !SourceASC) return;

        FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
        ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GrabbedEffectClass, 1.0f, ContextHandle);
        ActiveGrabbedEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    UAbilityTask_WaitGameplayEvent* WaitGrabEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Grab_End);
    WaitGrabEndEvent->EventReceived.AddDynamic(this, &UVbGA_Grab::EndGrab);
    WaitGrabEndEvent->ReadyForActivation();
}

void UVbGA_Grab::EndGrab(FGameplayEventData EventData)
{
    if (!K2_HasAuthority() || !TargetCharacter) return;

    if (TargetASC && ActiveGrabbedEffectHandle.IsValid())
    {
        TargetASC->RemoveActiveGameplayEffect(ActiveGrabbedEffectHandle);
    }

    TargetCharacter->LaunchCharacter(GetAvatarActorFromActorInfo()->GetActorForwardVector() * 1500, false, false);
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, VbGameplayTags::Event_Ability_Knockdown, FGameplayEventData());
}