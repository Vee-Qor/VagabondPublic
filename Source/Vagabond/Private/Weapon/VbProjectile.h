// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VbCoreTypes.h"
#include "GameFramework/Actor.h"
#include "VbProjectile.generated.h"

class UNiagaraComponent;
class UGameplayAbility;
class UGameplayEffect;
class UProjectileMovementComponent;

UCLASS()
class AVbProjectile : public AActor
{
    GENERATED_BODY()

public:
    AVbProjectile();

    void StartProjectile(const FVector& Direction, const FVbAttackContext& InAttackContext);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void TraceStep();
    void ClearTrace();

    // ==== Components ==== //
    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleDefaultsOnly)
    UNiagaraComponent* NiagaraSystem;

    UPROPERTY(VisibleDefaultsOnly)
    UProjectileMovementComponent* MovementComponent;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Config", meta=(Categories="Weapon.Attack"))
    FGameplayTag ImpactTypeTag;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float Speed = 12500.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float MaxSpeed = 12500.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float MaxDistance = 3000.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float TraceRadius = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float DistanceBetweenTraces = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float TraceInterval = 0.01f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    bool bDrawDebug = false;

    // ==== Runtime ==== ///
    FTimerHandle TraceTimerHandle;
    FVector PrevLocation;

    UPROPERTY()
    AActor* OwnerActor;

    UPROPERTY()
    TSet<AActor*> HitActors;

    UPROPERTY()
    FVbAttackContext AttackContext;
};