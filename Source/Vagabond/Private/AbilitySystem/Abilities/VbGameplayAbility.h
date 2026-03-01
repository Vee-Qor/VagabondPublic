// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "VbGameplayAbility.generated.h"

struct FVbAbilityParamsRow;
class AVbCharacter;
class UVbCombatComponent;
class UVbCharacterData;
class UVbEnemyData;
class UVbEnemyStyleData;
class UVbStyleData;
class UVbPlayerData;
class UVbPlayerStyleData;

UCLASS(Abstract)
class UVbGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGameplayAbility();

    const FGameplayTag& GetInputTag() const { return InputTag; }
    void SetInputTag(const FGameplayTag& InInputTag) { InputTag = InInputTag; }

protected:
    virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) const override;
    virtual const FGameplayTagContainer* GetCooldownTags() const override;

    void UpdateMotionWarpingTarget() const;
    AVbCharacter* GetVbCharacter() const;
    UVbCombatComponent* GetCombatComponent() const;
    const UVbCharacterData* GetCharacterData() const;
    const UVbPlayerData* GetPlayerData() const;
    const UVbEnemyData* GetEnemyData() const;
    const UVbStyleData* GetStyleData() const;
    const UVbEnemyStyleData* GetEnemyStyleData() const;
    const UVbPlayerStyleData* GetPlayerStyleData() const;

private:
    void FillCooldownInfo() const;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Input")
    FGameplayTag InputTag;

    // ==== Runtime ==== //
    UPROPERTY()
    mutable FScalableFloat CooldownDuration;

    UPROPERTY()
    mutable FGameplayTagContainer CooldownTags;

    UPROPERTY(Transient)
    FGameplayTagContainer TempCooldownTags;
};