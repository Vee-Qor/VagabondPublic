// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGAP_HitReact.generated.h"

class UNiagaraSystem;
struct FVbHitReactMontages;

UCLASS()
class UVbGAP_HitReact : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGAP_HitReact();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    void ExecuteTakenDamageCue(const FGameplayEventData* TriggerEventData);
    UAnimMontage* SelectHitReactMontage(const FGameplayEventData* TriggerEventData) const;
    FVbHitReactMontages GetHitReactMontages() const;
    bool HasHitReactImmune() const;
    
    void ReportSenseEvents(const FGameplayEventData* TriggerEventData);
};