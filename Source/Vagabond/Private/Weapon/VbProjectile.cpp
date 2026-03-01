// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Weapon/VbProjectile.h"

#include "Character/VbCombatComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"

AVbProjectile::AVbProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicatingMovement(true);

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComp"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetupAttachment(GetRootComponent());
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    NiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>("NiagaraSystem");
    NiagaraSystem->SetupAttachment(GetRootComponent());

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    MovementComponent->bRotationFollowsVelocity = true;
    MovementComponent->bAutoActivate = false;
    MovementComponent->ProjectileGravityScale = 0.0f;
}

void AVbProjectile::StartProjectile(const FVector& Direction, const FVbAttackContext& InAttackContext)
{
    if (!MovementComponent) return;

    AttackContext = InAttackContext;

    OwnerActor = GetOwner();
    if (!OwnerActor) return;

    MovementComponent->InitialSpeed = Speed;
    MovementComponent->MaxSpeed = MaxSpeed;
    MovementComponent->Velocity = Direction.GetSafeNormal() * MovementComponent->InitialSpeed;
    MovementComponent->Activate(true);

    SetLifeSpan(MaxDistance / MovementComponent->InitialSpeed);

    if (const UWorld* World = GetWorld())
    {
        PrevLocation = GetActorLocation();
        World->GetTimerManager().SetTimer(TraceTimerHandle, this, &AVbProjectile::TraceStep, TraceInterval, true);
    }
}

void AVbProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearTrace();

    Super::EndPlay(EndPlayReason);
}

void AVbProjectile::TraceStep()
{
    if (!OwnerActor) return;

    const UVbCombatComponent* CombatComponent = OwnerActor->FindComponentByClass<UVbCombatComponent>();
    if (!CombatComponent) return;

    const FVector TargetLocation = GetActorLocation();

    TArray<FHitResult> OutHitResults;

    FVbTraceParams Params;
    Params.TraceStart = PrevLocation;
    Params.TraceEnd = TargetLocation;
    Params.AlreadyHitActors = &HitActors;
    Params.DistanceBetweenTraces = DistanceBetweenTraces;
    Params.SphereRadius = TraceRadius;
    Params.bDrawDebug = bDrawDebug;
    Params.AdditionalIgnoreActor = this;

    bool bDidTraceStep = false;
    CombatComponent->DoSphereTraceStep(Params, OutHitResults, bDidTraceStep);

    if (bDidTraceStep)
    {
        PrevLocation = TargetLocation;
    }

    if (OutHitResults.Num() > 0)
    {
        AttackContext.ImpactTypeTag = ImpactTypeTag;
        CombatComponent->OnAttackHitDetected.Broadcast(OutHitResults[0], AttackContext);
        Destroy();
    }
}

void AVbProjectile::ClearTrace()
{
    if (TraceTimerHandle.IsValid())
    {
        if (const UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(TraceTimerHandle);
        }
    }
}