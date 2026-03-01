// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_EquipWeapon.generated.h"

UCLASS()
class UVbGA_EquipWeapon : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_EquipWeapon();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UAnimMontage* GetEquipMontage() const;

    UFUNCTION()
    void EquipWeaponEvent(FGameplayEventData EventData);
};