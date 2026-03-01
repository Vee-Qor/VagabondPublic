// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Calculations/VbGEEC_Damage.h"

#include "AbilitySystem/VbAttributeSet.h"
#include "VbGameplayTags.h"

UVbGEEC_Damage::UVbGEEC_Damage()
{
    AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(UVbAttributeSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source,
        false);
    CriticalChanceDef = FGameplayEffectAttributeCaptureDefinition(UVbAttributeSet::GetCriticalChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source,
        false);
    CriticalMultiplierDef = FGameplayEffectAttributeCaptureDefinition(UVbAttributeSet::GetCriticalMultiplierAttribute(),
        EGameplayEffectAttributeCaptureSource::Source, false);

    ArmorDef = FGameplayEffectAttributeCaptureDefinition(UVbAttributeSet::GetArmorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

    RelevantAttributesToCapture.Add(AttackPowerDef);
    RelevantAttributesToCapture.Add(CriticalChanceDef);
    RelevantAttributesToCapture.Add(CriticalMultiplierDef);
    RelevantAttributesToCapture.Add(ArmorDef);
}

void UVbGEEC_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    float Damage = GetSetByCallerSafe(Spec, VbGameplayTags::Data_Damage);
    if (Damage <= 0.0f) return;

    const FAggregatorEvaluateParameters Params;

    float AttackPower = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttackPowerDef, Params, AttackPower);
    AttackPower = FMath::Max(0.0f, AttackPower);

    float CriticalChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CriticalChanceDef, Params, CriticalChance);
    CriticalChance = FMath::Max(0.0f, CriticalChance);

    if (CriticalChance > 1.0f)
    {
        CriticalChance *= 0.01f;
    }

    CriticalChance = FMath::Clamp(CriticalChance, 0.0f, 1.0f);

    float CriticalMultiplier = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CriticalMultiplierDef, Params, CriticalMultiplier);

    float Armor = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ArmorDef, Params, Armor);
    Armor = FMath::Max(0.0f, Armor);

    Damage *= AttackPower;

    if (const bool bIsCritical = FMath::FRand() < CriticalChance)
    {
        Damage *= CriticalMultiplier;
    }

    const float Mitigation = 100.0f / (100.0f + Armor);
    const float FinalDamage = Damage * Mitigation;

    if (FinalDamage <= 0.0f) return;

    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVbAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVbAttributeSet::GetPoiseAttribute(), EGameplayModOp::Additive, -FinalDamage));
}

float UVbGEEC_Damage::GetSetByCallerSafe(const FGameplayEffectSpec& EffectSpec, const FGameplayTag& Tag, float DefaultValue)
{
    return EffectSpec.GetSetByCallerMagnitude(Tag, false, DefaultValue);
}