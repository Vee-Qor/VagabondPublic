// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Animation/Notifies/VbANS_AttackTraceSetup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "VbCoreTypes.h"

FString UVbANS_AttackTraceSetup::GetNotifyName_Implementation() const
{
    return "AttackTraceSetup";
}

void UVbANS_AttackTraceSetup::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp || !MeshComp->GetOwner() || !MeshComp->GetOwner()->HasAuthority()) return;
    if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

    ActivePayload = NewObject<UVbAttackTracePayload>(MeshComp->GetOwner());
    ActivePayload->SocketName = SocketName;
    ActivePayload->BoxSize = BoxSize;
    ActivePayload->DistanceBetweenTraces = DistanceBetweenTraces;
    ActivePayload->Interval = Interval;
    ActivePayload->ImpactTypeTag = ImpactTypeTag;
    ActivePayload->bDrawDebug = bDrawDebug;

    FGameplayEventData EventData;
    EventData.OptionalObject = ActivePayload;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), TraceStartTag, EventData);
}

void UVbANS_AttackTraceSetup::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (!ActivePayload) return;
    if (!MeshComp || !MeshComp->GetOwner() || !MeshComp->GetOwner()->HasAuthority()) return;
    if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

    FGameplayEventData EventData;
    EventData.OptionalObject = ActivePayload;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), TraceEndTag, EventData);

    ActivePayload = nullptr;
}