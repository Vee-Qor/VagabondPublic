// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbGameplayAbility.h"
#include "VbGA_TripleShot.generated.h"

class AVbProjectile;

UCLASS()
class UVbGA_TripleShot : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_TripleShot();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    FVector GetLaunchDirection() const;
    void StartCharge();
    void UpdateCharge();
    void StopCharge();

    UFUNCTION()
    void OnInputReleased(float TimeHeld);

    UFUNCTION()
    void OnAttackEnd(FGameplayEventData EventData);

    UFUNCTION()
    void SpawnProjectile(FGameplayEventData EventData);

    UFUNCTION()
    void StartProjectile(FGameplayEventData EventData);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* TripleShotMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName EndSectionName = "End";

    UPROPERTY(EditDefaultsOnly)
    float ProjectileOffset = 25.0f;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AVbProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly)
    FName SpawnSocket;

    UPROPERTY(EditDefaultsOnly)
    float SpawnProjectileDelay = 0.5f;

    int32 MaxProjectiles = 3;

    // ==== Runtime ==== //
    FTimerHandle ChargeTimerHandle;

    UPROPERTY()
    TArray<AVbProjectile*> SpawnedProjectiles;

    UPROPERTY()
    AActor* Avatar;
};