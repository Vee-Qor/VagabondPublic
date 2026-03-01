// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/TargetActor/VbTargetActor_GroundArea.h"

#include "Components/DecalComponent.h"

AVbTargetActor_GroundArea::AVbTargetActor_GroundArea()
{
    PrimaryActorTick.bCanEverTick = true;
    bDestroyOnConfirmation = true;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
    
    DecalComponent = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
    DecalComponent->SetupAttachment(GetRootComponent());
}

void AVbTargetActor_GroundArea::ConfirmTargetingAndContinue()
{
    FGameplayAbilityTargetDataHandle Handle;

    FGameplayAbilityTargetData_LocationInfo* Data = new FGameplayAbilityTargetData_LocationInfo();
    Data->TargetLocation.LiteralTransform.SetLocation(GetActorLocation());
    Handle.Add(Data);

    TargetDataReadyDelegate.Broadcast(Handle);

    if (bDestroyOnConfirmation)
    {
        Destroy();
    }
}

void AVbTargetActor_GroundArea::SetTargetAreaRadius(float AreaRadius)
{
    TargetAreaRadius = AreaRadius;
    DecalComponent->DecalSize = FVector(AreaRadius);
}

void AVbTargetActor_GroundArea::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
    {
        SetActorLocation(GetTargetPoint());
    }
}

FVector AVbTargetActor_GroundArea::GetTargetPoint() const
{
    if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();

    FVector ViewLocation;
    FRotator ViewRotation;
    PrimaryPC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * TargetTraceRange;

    if (const UWorld* World = GetWorld())
    {
        FHitResult TraceResult;
        World->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Visibility);

        if (!TraceResult.bBlockingHit)
        {
            World->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Visibility);
        }

        if (!TraceResult.bBlockingHit)
        {
            return GetActorLocation();
        }

        return TraceResult.ImpactPoint;
    }

    return GetActorLocation();
}