// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbCoreTypes.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_FinalCut.generated.h"

UCLASS()
class UVbGA_FinalCut : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_FinalCut();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    void StartCharge();
    void UpdateDamage();
    void StopCharge();

    void StartFinalCut();

    void DetachCamera() const;
    void AttachCamera() const;
    void SetDamageImmuneEnabled(bool bEnabled) const;

    UFUNCTION()
    void OnTraceEndEvent(FGameplayEventData EventData);

    UFUNCTION()
    void OnInputReleased(float TimeHeld);

    UPROPERTY(EditDefaultsOnly, Category="Animation|Montage")
    UAnimMontage* FinalCutMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation|Montage")
    TSubclassOf<UGameplayEffect> DamageMultiplierEffectClass;

    UPROPERTY(EditDefaultsOnly, Category="Animation|Montage")
    TSubclassOf<UGameplayEffect> DamageImmuneClass;

    UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(Categories="Combat.Attack.Type"))
    FGameplayTag ImpactTypeTag;

    UPROPERTY(EditDefaultsOnly, Category="Animation|Section")
    FName LoopSectionName = "Loop";

    UPROPERTY(EditDefaultsOnly, Category="Animation|Section")
    FName EndSectionName = "End";

    UPROPERTY(EditDefaultsOnly, Category="Damage")
    float IncreaseInterval = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category="Damage")
    int32 MaxIncreaseCount = 3;

    UPROPERTY(EditDefaultsOnly, Category="Trace")
    FVbAreaTraceInfo AreaTraceInfo;

    // ==== Runtime ==== //
    UPROPERTY()
    FActiveGameplayEffectHandle ChargeAttackPowerEffectClass;

    FTimerHandle CutTimerHandle;
    FTimerHandle UpdateDamageTimerHandle;

    int32 IncreaseDamageIndex = 0;

    UPROPERTY()
    FVbAttackContext AttackContext;
};