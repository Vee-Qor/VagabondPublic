// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbGA_Attack_Melee.h"
#include "VbGA_MillionStrikes.generated.h"

UCLASS()
class UVbGA_MillionStrikes : public UVbGA_Attack_Melee
{
    GENERATED_BODY()

public:
    UVbGA_MillionStrikes();
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack() override;

private:
    UFUNCTION()
    void OnInputPressed(float TimeWaited);

    void CheckLoopExit();
    void RequestExitLoop() const;
    void TryProcessClick();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* MillionStrikesMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName LoopSectionName = "Loop";

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName EndSectionName = "End";

    UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(ClampMin="0.0"))
    float MaxAttackSpeed = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category="Clicks", meta=(ClampMin="1"))
    int32 ClicksToMax = 8;

    UPROPERTY(EditDefaultsOnly, Category="Clicks", meta=(ClampMin="0.0"))
    float ClickCooldown = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category="Duration", meta=(ClampMin="0.0"))
    float MinLoopDuration = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category="Duration", meta=(ClampMin="0.0"))
    float LoopExtendPerClick = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category="Duration", meta=(ClampMin="0.0"))
    float BonusTimeAtMaxSpeed = 3.0f;

    // ==== Runtime ==== //
    FTimerHandle LoopCheckTimer;
    float LoopStartTime = 0.0f;
    float LoopEndTime = 0.0f;
    float NextAllowedClickTime = 0.0f;
    float BaseAttackSpeed = 0.0f;

    int32 ValidClickCount = 0;
    bool bMaxSpeedReached = false;
    bool bExitRequested = false;
};