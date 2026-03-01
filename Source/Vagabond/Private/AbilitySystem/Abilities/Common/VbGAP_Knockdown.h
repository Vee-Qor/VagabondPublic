// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGAP_Knockdown.generated.h"

UCLASS()
class UVbGAP_Knockdown : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGAP_Knockdown();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    void RemoveKnockdownEffect();
    
    UFUNCTION()
    void OnEndKnockdown(FGameplayEventData EventData);

    UPROPERTY()
    FActiveGameplayEffectHandle ActiveKnockdownEffectHandle;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> KnockdownEffectClass;
};