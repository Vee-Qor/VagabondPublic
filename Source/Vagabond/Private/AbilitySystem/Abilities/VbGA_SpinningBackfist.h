// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/VbGA_Attack_Melee.h"
#include "VbGA_SpinningBackfist.generated.h"

UCLASS()
class UVbGA_SpinningBackfist : public UVbGA_Attack_Melee
{
    GENERATED_BODY()

public:
    UVbGA_SpinningBackfist();
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack() override;
    virtual FVbAttackContext BuildAttackContext(const UVbAttackTracePayload* Payload) override;

private:
    UFUNCTION()
    void OnStunEventReceived(FGameplayEventData EventData);

    UPROPERTY(EditDefaultsOnly, Category="Effect")
    TSubclassOf<UGameplayEffect> DamageImmuneClass;

    UPROPERTY(EditDefaultsOnly, Category="Effect")
    TSubclassOf<UGameplayEffect> StunEffectClass;
};