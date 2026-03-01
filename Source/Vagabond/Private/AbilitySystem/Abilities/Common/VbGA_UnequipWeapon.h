// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_UnequipWeapon.generated.h"

UCLASS()
class UVbGA_UnequipWeapon : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_UnequipWeapon();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UAnimMontage* GetUnequipMontage() const;

    UFUNCTION()
    void UnequipWeaponEvent(FGameplayEventData EventData);
};