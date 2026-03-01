// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Calculations/VbMMC_Cooldown.h"

#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbCoreTypes.h"
#include "Framework/VbFunctionLibrary.h"

float UVbMMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    const UVbGameplayAbility* Ability = Cast<UVbGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
    if (!Ability) return 0.0f;
    
    const FVbAbilityParamsRow* AbilityParamsRow = UVbFunctionLibrary::GetAbilityParamsRow(Ability->GetWorld(), Ability->GetClass());
    if (!AbilityParamsRow) return 0.0f;
    
    return AbilityParamsRow->Cooldown.GetValueAtLevel(Ability->GetAbilityLevel());
}