// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Attack_Melee.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "VbCoreTypes.h"
#include "Character/VbCombatComponent.h"
#include "VbGameplayTags.h"
#include "Framework/VbFunctionLibrary.h"

UVbGA_Attack_Melee::UVbGA_Attack_Melee()
{
    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_Melee_Light.GetTag().GetSingleTagContainer());
}

void UVbGA_Attack_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (const UWorld* World = GetWorld())
    {
        for (auto& Pair : TraceSessions)
        {
            World->GetTimerManager().ClearTimer(Pair.Value.TimerHandle);
        }
    }

    TraceSessions.Reset();
    OwnerMesh = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_Attack_Melee::ExecuteAttack()
{
    Super::ExecuteAttack();

    UAbilityTask_WaitGameplayEvent* WaitAttackTraceStartEvent =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_TraceStart);
    WaitAttackTraceStartEvent->EventReceived.AddDynamic(this, &UVbGA_Attack_Melee::StartTrace);
    WaitAttackTraceStartEvent->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitAttackTraceEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_TraceEnd);
    WaitAttackTraceEndEvent->EventReceived.AddDynamic(this, &UVbGA_Attack_Melee::TraceEnd);
    WaitAttackTraceEndEvent->ReadyForActivation();
}

FVbAttackContext UVbGA_Attack_Melee::BuildAttackContext(const UVbAttackTracePayload* Payload)
{
    FVbAttackContext AttackContext;
    AttackContext.Damage = UVbFunctionLibrary::GetAbilityDamage(GetWorld(), GetClass()).GetValueAtLevel(GetAbilityLevel());
    if (Payload)
    {
        AttackContext.ImpactTypeTag = Payload->ImpactTypeTag;
    }
    if (const UVbStyleData* StyleData = GetStyleData())
    {
        AttackContext.StyleTag = StyleData->GetStyleTag();
    }

    return AttackContext;
}

void UVbGA_Attack_Melee::StartTrace(FGameplayEventData EventData)
{
    const UVbAttackTracePayload* Payload = Cast<UVbAttackTracePayload>(EventData.OptionalObject);
    if (!Payload) return;

    OwnerMesh = GetOwningComponentFromActorInfo();
    if (!OwnerMesh) return;

    FVbTraceSession& Session = TraceSessions.FindOrAdd(Payload);
    Session.PrevLocation = OwnerMesh->GetSocketLocation(Payload->SocketName);
    Session.HitActors.Reset();

    if (const UWorld* World = GetWorld())
    {
        const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UVbGA_Attack_Melee::TraceStep, Payload);
        World->GetTimerManager().SetTimer(Session.TimerHandle, TimerDelegate, Payload->Interval, true, Payload->Interval);
    }
}

void UVbGA_Attack_Melee::TraceStep(const UVbAttackTracePayload* Payload)
{
    FVbTraceSession* Session = TraceSessions.Find(Payload);
    if (!Payload || !Session || !OwnerMesh) return;

    const FVector TargetLocation = OwnerMesh->GetSocketLocation(Payload->SocketName);
    const FRotator SocketRotation = OwnerMesh->GetSocketRotation(Payload->SocketName);

    TArray<FHitResult> OutHitResults;

    FVbTraceParams Params;
    Params.TraceStart = Session->PrevLocation;
    Params.TraceEnd = TargetLocation;
    Params.AlreadyHitActors = &Session->HitActors;
    Params.DistanceBetweenTraces = Payload->DistanceBetweenTraces;
    Params.BoxSize = Payload->BoxSize;
    Params.BoxRotation = SocketRotation;
    Params.bDrawDebug = Payload->bDrawDebug;

    const UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return;

    bool bDidTraceStep = false;
    CombatComponent->DoBoxTraceStep(Params, OutHitResults, bDidTraceStep);

    if (bDidTraceStep)
    {
        Session->PrevLocation = TargetLocation;
    }

    const FVbAttackContext& AttackContext = BuildAttackContext(Payload);

    for (const FHitResult& HitResult : OutHitResults)
    {
        CombatComponent->OnAttackHitDetected.Broadcast(HitResult, AttackContext);
    }
}

void UVbGA_Attack_Melee::TraceEnd(FGameplayEventData EventData)
{
    const UVbAttackTracePayload* Payload = Cast<UVbAttackTracePayload>(EventData.OptionalObject);
    if (!Payload) return;

    if (const UWorld* World = GetWorld())
    {
        if (FVbTraceSession* Session = TraceSessions.Find(Payload))
        {
            World->GetTimerManager().ClearTimer(Session->TimerHandle);
        }
    }

    TraceSessions.Remove(Payload);
}