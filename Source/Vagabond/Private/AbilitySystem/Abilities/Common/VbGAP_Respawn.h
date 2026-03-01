// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGAP_Respawn.generated.h"

UCLASS()
class UVbGAP_Respawn : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGAP_Respawn();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UAnimMontage* GetRespawnMontage(const FGameplayEventData* TriggerEventData) const;
    void Respawn() const;

    UFUNCTION()
    void EndRespawning(FGameplayEventData EventData);

    UPROPERTY(EditDefaultsOnly, Category="Effect")
    TSubclassOf<UGameplayEffect> RespawnEffect;
};