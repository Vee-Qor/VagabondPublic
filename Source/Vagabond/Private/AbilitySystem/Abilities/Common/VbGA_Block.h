// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_Block.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class UVbGA_Block : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_Block();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    void RemoveParryingEffect();
    
    UFUNCTION()
    void OnBlockedHit(FGameplayEventData EventData);
    
    UFUNCTION()
    void OnHitMontageFinished();
    
    UFUNCTION()
    void OnInputReleased(float TimeHeld);

    UFUNCTION()
    void OnParryingStart(FGameplayEventData EventData);

    UFUNCTION()
    void OnParryingEnd(FGameplayEventData EventData);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Animation")
    TSubclassOf<UGameplayEffect> ParryingEffectClass;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* BlockMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* HitMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName StartSection = "Start";
    
    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName LoopSection = "Loop";

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName EndSection = "End";

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName HitSection = "Hit";

    // ==== Runtime ==== //
    FActiveGameplayEffectHandle ActiveParryingEffectHandle;
};