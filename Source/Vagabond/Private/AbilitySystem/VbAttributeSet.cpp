// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/VbAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UVbAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }

    if (Attribute == GetResourceAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxResource());
    }

    if (Attribute == GetPoiseAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxPoise());
    }

    if (Attribute == GetLevelAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxLevel());
    }
}

void UVbAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(GetHealth());
    }

    if (Data.EvaluatedData.Attribute == GetResourceAttribute())
    {
        SetResource(GetResource());
    }

    if (Data.EvaluatedData.Attribute == GetPoiseAttribute())
    {
        SetPoise(GetPoise());
    }

    if (Data.EvaluatedData.Attribute == GetLevelAttribute())
    {
        SetLevel(GetLevel());
    }
}

void UVbAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Resource, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, MaxResource, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Poise, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, MaxPoise, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, CriticalMultiplier, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, Experience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, NextExperience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, PrevExperience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UVbAttributeSet, MaxExperience, COND_None, REPNOTIFY_Always);
}

void UVbAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Health, OldHealth);
}

void UVbAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, MaxHealth, OldMaxHealth);
}

void UVbAttributeSet::OnRep_Resource(const FGameplayAttributeData& OldResource) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Resource, OldResource);
}

void UVbAttributeSet::OnRep_MaxResource(const FGameplayAttributeData& OldMaxResource) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, MaxResource, OldMaxResource);
}

void UVbAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Poise, OldPoise);
}

void UVbAttributeSet::OnRep_MaxPoise(const FGameplayAttributeData& OldMaxPoise) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, MaxPoise, OldMaxPoise);
}

void UVbAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, AttackPower, OldAttackPower);
}

void UVbAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, CriticalChance, OldCriticalChance);
}

void UVbAttributeSet::OnRep_CriticalMultiplier(const FGameplayAttributeData& OldCriticalMultiplier) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, CriticalMultiplier, OldCriticalMultiplier);
}

void UVbAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Armor, OldArmor);
}

void UVbAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Level, OldLevel);
}

void UVbAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, MaxLevel, OldMaxLevel);
}

void UVbAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldExperience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, Experience, OldExperience);
}

void UVbAttributeSet::OnRep_PrevExperience(const FGameplayAttributeData& OldPrevExperience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, PrevExperience, OldPrevExperience);
}

void UVbAttributeSet::OnRep_NextExperience(const FGameplayAttributeData& OldNextExperience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, NextExperience, OldNextExperience);
}

void UVbAttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UVbAttributeSet, MaxExperience, OldMaxExperience);
}