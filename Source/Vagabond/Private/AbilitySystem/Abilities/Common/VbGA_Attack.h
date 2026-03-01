// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_Attack.generated.h"

UCLASS(Abstract)
class UVbGA_Attack : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_Attack();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack();

private:
    void ResetCombo();

    UFUNCTION()
    void OnAttackEnd(FGameplayEventData EventData);

    UPROPERTY(EditDefaultsOnly)
    TArray<UAnimMontage*> AttackMontages;

    UPROPERTY(EditDefaultsOnly)
    float ComboResetDelay = 0.4f;

    FTimerHandle ResetComboTimerHandle;
    int32 ComboIndex = 0;
};