// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGAP_Death.generated.h"

UCLASS()
class UVbGAP_Death : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGAP_Death();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UAnimMontage* GetDeathMontage(const FGameplayEventData* TriggerEventData) const;

    UFUNCTION()
    void ApplyDeathStatus(FGameplayEventData EventData);

    void DeathReward(const FGameplayEventData* TriggerEventData);
    TArray<AActor*> GetRewardTargets() const;
    
    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Reward")
    TSubclassOf<UGameplayEffect> RewardEffectClass;

    UPROPERTY(EditDefaultsOnly, Category="Reward")
    float RangeReward = 10000.0f;

    UPROPERTY(EditDefaultsOnly, Category="Reward")
    float ExperienceReward = 200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    float ExperienceRewardPerExperience = 0.1f;
};