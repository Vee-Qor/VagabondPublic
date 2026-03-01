// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_Dodge.generated.h"

class UCharacterMovementComponent;

UCLASS()
class UVbGA_Dodge : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_Dodge();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UAnimMontage* SelectDodgeMontage() const;

    UFUNCTION()
    void OnEvadingStart(FGameplayEventData EventData);

    UFUNCTION()
    void OnEvadingEnd(FGameplayEventData EventData);

    void ApplyInvulnerability();
    void RemoveInvulnerability();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Effects")
    TSubclassOf<UGameplayEffect> InvulnerabilityEffectClass;

    UPROPERTY(EditDefaultsOnly, Category="Effects")
    float DodgeStrength = 1000.0f;

    UPROPERTY(EditDefaultsOnly, Category="Effects")
    float DodgeDuration = 0.7f;

    // ==== Runtime ==== //
    UPROPERTY()
    AActor* Avatar;

    UPROPERTY()
    UCharacterMovementComponent* MovementComponent;

    UPROPERTY()
    FActiveGameplayEffectHandle InvulnerabilityEffectHandle;
};