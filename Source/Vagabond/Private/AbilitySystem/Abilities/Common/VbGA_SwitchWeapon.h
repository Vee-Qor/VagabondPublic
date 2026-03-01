// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_SwitchWeapon.generated.h"

UCLASS()
class UVbGA_SwitchWeapon : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_SwitchWeapon();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};