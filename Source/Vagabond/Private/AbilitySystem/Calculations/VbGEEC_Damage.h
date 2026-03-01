// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "VbGEEC_Damage.generated.h"

UCLASS()
class UVbGEEC_Damage : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UVbGEEC_Damage();
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
    static float GetSetByCallerSafe(const FGameplayEffectSpec& EffectSpec, const FGameplayTag& Tag, float DefaultValue = 0.0f);

    FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
    FGameplayEffectAttributeCaptureDefinition CriticalChanceDef;
    FGameplayEffectAttributeCaptureDefinition CriticalMultiplierDef;
    FGameplayEffectAttributeCaptureDefinition ArmorDef;
};