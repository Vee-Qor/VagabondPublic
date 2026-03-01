// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VbAttributeSet.generated.h"

UCLASS()
class UVbAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Health);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, MaxHealth);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Resource);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, MaxResource);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Poise);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, MaxPoise);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, AttackPower);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, CriticalChance);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, CriticalMultiplier);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Armor);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Damage);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Level);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, MaxLevel);
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, Experience)
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, PrevExperience)
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, NextExperience)
    ATTRIBUTE_ACCESSORS_BASIC(UVbAttributeSet, MaxExperience)

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(ReplicatedUsing=OnRep_Health)
    FGameplayAttributeData Health;

    UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;

    UPROPERTY(ReplicatedUsing=OnRep_Resource)
    FGameplayAttributeData Resource;

    UPROPERTY(ReplicatedUsing=OnRep_MaxResource)
    FGameplayAttributeData MaxResource;

    UPROPERTY(ReplicatedUsing=OnRep_Poise)
    FGameplayAttributeData Poise;

    UPROPERTY(ReplicatedUsing=OnRep_MaxPoise)
    FGameplayAttributeData MaxPoise;

    UPROPERTY(ReplicatedUsing=OnRep_AttackPower)
    FGameplayAttributeData AttackPower;

    UPROPERTY(ReplicatedUsing=OnRep_CriticalChance)
    FGameplayAttributeData CriticalChance;

    UPROPERTY(ReplicatedUsing=OnRep_CriticalMultiplier)
    FGameplayAttributeData CriticalMultiplier;

    UPROPERTY(ReplicatedUsing=OnRep_Armor)
    FGameplayAttributeData Armor;

    UPROPERTY(ReplicatedUsing=OnRep_Level)
    FGameplayAttributeData Level;

    UPROPERTY(ReplicatedUsing=OnRep_MaxLevel)
    FGameplayAttributeData MaxLevel;

    UPROPERTY(ReplicatedUsing=OnRep_Experience)
    FGameplayAttributeData Experience;

    UPROPERTY(ReplicatedUsing=OnRep_PrevExperience)
    FGameplayAttributeData PrevExperience;

    UPROPERTY(ReplicatedUsing=OnRep_NextExperience)
    FGameplayAttributeData NextExperience;

    UPROPERTY(ReplicatedUsing=OnRep_MaxExperience)
    FGameplayAttributeData MaxExperience;

    UPROPERTY()
    FGameplayAttributeData Damage;

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

    UFUNCTION()
    void OnRep_Resource(const FGameplayAttributeData& OldResource) const;

    UFUNCTION()
    void OnRep_MaxResource(const FGameplayAttributeData& OldMaxResource) const;

    UFUNCTION()
    void OnRep_Poise(const FGameplayAttributeData& OldPoise) const;

    UFUNCTION()
    void OnRep_MaxPoise(const FGameplayAttributeData& OldMaxPoise) const;

    UFUNCTION()
    void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) const;

    UFUNCTION()
    void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance) const;

    UFUNCTION()
    void OnRep_CriticalMultiplier(const FGameplayAttributeData& OldCriticalMultiplier) const;

    UFUNCTION()
    void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

    UFUNCTION()
    void OnRep_Level(const FGameplayAttributeData& OldLevel) const;

    UFUNCTION()
    void OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel) const;

    UFUNCTION()
    void OnRep_Experience(const FGameplayAttributeData& OldExperience) const;

    UFUNCTION()
    void OnRep_PrevExperience(const FGameplayAttributeData& OldPrevExperience) const;

    UFUNCTION()
    void OnRep_NextExperience(const FGameplayAttributeData& OldNextExperience) const;

    UFUNCTION()
    void OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience) const;
};