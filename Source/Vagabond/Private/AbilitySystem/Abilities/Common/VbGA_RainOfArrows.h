// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbCoreTypes.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGA_RainOfArrows.generated.h"

class AVbProjectile;
class AVbTargetActor_GroundArea;
class UNiagaraComponent;
struct FVbAreaTraceInfo;

UCLASS()
class UVbGA_RainOfArrows : public UVbGameplayAbility
{
    GENERATED_BODY()

public:
    UVbGA_RainOfArrows();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UFUNCTION()
    void TargetConfirmed(const FGameplayAbilityTargetDataHandle& Data);

    UFUNCTION()
    void TargetCanceled(const FGameplayAbilityTargetDataHandle& Data);

    UFUNCTION()
    void OnAttackEnd(FGameplayEventData EventData);

    UFUNCTION()
    void OnSpawnProjectile(FGameplayEventData EventData);

    UFUNCTION()
    void OnStartProjectile(FGameplayEventData EventData);

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* RainOfArrowsMontage;

    UPROPERTY(EditDefaultsOnly, Category="TargetActor")
    TSubclassOf<AVbTargetActor_GroundArea> TargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(Categories="Combat.Attack.Type"))
    FGameplayTag ImpactTypeTag;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AVbProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FName SpawnProjectileSocket = "Arrow";

    UPROPERTY(EditDefaultsOnly, Category="Trace")
    FVbAreaTraceInfo AreaTraceInfo;

    // ==== Runtime ==== //
    UPROPERTY()
    AVbProjectile* Projectile;

    FVector TargetActorPoint;
};