// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbFootstepComponent.h"

#include "VbCharacter.h"
#include "Data/VbCharacterData.h"
#include "Framework/VbFootstepConfig.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UVbFootstepComponent::UVbFootstepComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVbFootstepComponent::HandleFootStep(const FVbFootstepContext& Context) const
{
    const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;

    const USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    if (!Mesh) return;

    FVector FootLocation = FVector::ZeroVector;
    float SocketOffset = DefaultSocketOffset;

    switch (Context.Foot)
    {
        case EVbFoot::Left: FootLocation = Mesh->GetSocketLocation(LeftFootSocketName);
            break;
        case EVbFoot::Right: FootLocation = Mesh->GetSocketLocation(RightFootSocketName);
            break;
        case EVbFoot::Both: FootLocation = Mesh->GetSocketLocation(BothFootSocketName);
            SocketOffset = BothSocketOffset;
            break;
    }

    const UWorld* World = GetWorld();
    const FVector TraceStart = FootLocation;
    const FVector TraceEnd = FootLocation - FVector::UpVector * SocketOffset;

    FHitResult HitResult;
    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.bReturnPhysicalMaterial = true;
    CollisionQueryParams.AddIgnoredActor(OwnerCharacter);

    if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, CollisionQueryParams))
    {
        if (!HitResult.PhysMaterial.IsValid()) return;

        const UVbFootstepConfig* FootstepConfig = GetFootstepConfig();
        if (!FootstepConfig) return;

        const TMap<TEnumAsByte<EPhysicalSurface>, FVbFootstepType>& FootstepSoundMap = FootstepConfig->GetFootstepSoundMap();
        const FVbFootstepType* FootstepType = FootstepSoundMap.Find(HitResult.PhysMaterial->SurfaceType);
        if (!FootstepType) return;

        USoundBase* FootstepSound = nullptr;

        switch (Context.FootstepMovement)
        {
            case EVbFootstepMovement::Walk: FootstepSound = FootstepType->WalkFootstepCue;
                break;
            case EVbFootstepMovement::WalkStop: FootstepSound = FootstepType->WalkStopFootstepCue;
                break;
            case EVbFootstepMovement::Jog: FootstepSound = FootstepType->JogFootstepCue;
                break;
            case EVbFootstepMovement::JogStop: FootstepSound = FootstepType->JogStopFootstepCue;
                break;
            case EVbFootstepMovement::Run: FootstepSound = FootstepType->RunFootstepCue;
                break;
            case EVbFootstepMovement::RunStop: FootstepSound = FootstepType->RunStopFootstepCue;
                break;
            case EVbFootstepMovement::Jump: FootstepSound = FootstepType->JumpFootstepCue;
                break;
            case EVbFootstepMovement::Land: FootstepSound = FootstepType->LandFootstepCue;
                break;
        }

        if (!FootstepSound) return;

        if (bDrawDebug)
        {
            DrawDebugSphere(World, HitResult.ImpactPoint, 16.0f, 16, FColor::Red, false, 2.0f);
            DrawDebugString(World, HitResult.ImpactPoint, HitResult.PhysMaterial->GetName(), nullptr, FColor::White, 2.0f);
        }

        UGameplayStatics::PlaySoundAtLocation(World, FootstepSound, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
    }
}

UVbFootstepConfig* UVbFootstepComponent::GetFootstepConfig() const
{
    if (const AVbCharacter* OwnerCharacter = Cast<AVbCharacter>(GetOwner()))
    {
        const UVbCharacterData* CharacterData = OwnerCharacter->GetCharacterData();

        return CharacterData ? CharacterData->GetFootstepConfig() : nullptr;
    }

    return nullptr;
}