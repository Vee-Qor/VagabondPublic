// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/Common/VbGA_Attack.h"
#include "CoreMinimal.h"
#include "Data/VbStyleData.h"
#include "VbGA_Attack_Projectile.generated.h"

class AVbProjectile;

UCLASS()
class UVbGA_Attack_Projectile : public UVbGA_Attack
{
    GENERATED_BODY()

public:
    UVbGA_Attack_Projectile();
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    virtual void ExecuteAttack() override;

private:
    FVector GetLaunchDirection() const;

    UFUNCTION()
    void SpawnProjectile(FGameplayEventData EventData);

    UFUNCTION()
    void OnStartProjectile(FGameplayEventData EventData);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AVbProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly)
    FName SpawnSocket;

    // ==== Runtime ==== //
    UPROPERTY()
    AVbProjectile* Projectile;

    UPROPERTY()
    AActor* Avatar;
};