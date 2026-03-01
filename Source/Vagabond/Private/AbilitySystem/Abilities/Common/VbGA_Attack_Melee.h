// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Common/VbGA_Attack.h"
#include "Data/VbStyleData.h"
#include "VbGA_Attack_Melee.generated.h"

USTRUCT()
struct FVbTraceSession
{
    GENERATED_BODY()

    FTimerHandle TimerHandle;
    FVector PrevLocation = FVector::ZeroVector;

    UPROPERTY()
    TSet<AActor*> HitActors;
};

class UVbAttackTracePayload;

UCLASS()
class UVbGA_Attack_Melee : public UVbGA_Attack
{
    GENERATED_BODY()

public:
    UVbGA_Attack_Melee();
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack() override;
    virtual FVbAttackContext BuildAttackContext(const UVbAttackTracePayload* Payload);

private:
    void TraceStep(const UVbAttackTracePayload* Payload);

    UFUNCTION()
    void StartTrace(FGameplayEventData EventData);

    UFUNCTION()
    void TraceEnd(FGameplayEventData EventData);

    // ==== Runtime ==== //
    UPROPERTY()
    TMap<const UVbAttackTracePayload*, FVbTraceSession> TraceSessions;

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;
};