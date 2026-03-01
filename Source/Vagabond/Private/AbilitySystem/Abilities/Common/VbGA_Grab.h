// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_Attack_Melee.h"
#include "VbGA_Grab.generated.h"

UCLASS()
class UVbGA_Grab : public UVbGA_Attack_Melee
{
    GENERATED_BODY()

public:
    UVbGA_Grab();
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack() override;
    virtual FVbAttackContext BuildAttackContext(const UVbAttackTracePayload* Payload) override;

private:
    UFUNCTION()
    void StartGrab(FGameplayEventData EventData);

    UFUNCTION()
    void EndGrab(FGameplayEventData EventData);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> GrabbedEffectClass;

    UPROPERTY(EditDefaultsOnly)
    FName GrabSocket = "GrabSocket";

    // ==== Runtime ==== //
    UPROPERTY()
    AVbCharacter* TargetCharacter;

    UPROPERTY()
    UAbilitySystemComponent* TargetASC;

    UPROPERTY()
    FActiveGameplayEffectHandle ActiveGrabbedEffectHandle;
};