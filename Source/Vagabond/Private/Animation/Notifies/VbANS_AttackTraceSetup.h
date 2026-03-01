// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "VbGameplayTags.h"
#include "VbANS_AttackTraceSetup.generated.h"

class UVbAttackTracePayload;

UCLASS()
class UVbANS_AttackTraceSetup : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual FString GetNotifyName_Implementation() const override;
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
        const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category="TraceInfo")
    FGameplayTag TraceStartTag = VbGameplayTags::Event_Attack_TraceStart;

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    FName SocketName = "weapon_r";

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    FVector BoxSize = FVector(80.0f, 50.0f, 160.0f);

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    float DistanceBetweenTraces = 60.0f;

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    float Interval = 0.01f;

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, Category="TraceInfo")
    FGameplayTag TraceEndTag = VbGameplayTags::Event_Attack_TraceEnd;

    UPROPERTY(EditAnywhere, Category="TraceInfo", meta=(Categories="Weapon.Attack"))
    FGameplayTag ImpactTypeTag;

    UPROPERTY()
    UVbAttackTracePayload* ActivePayload;
};