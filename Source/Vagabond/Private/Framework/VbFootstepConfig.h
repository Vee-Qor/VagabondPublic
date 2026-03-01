// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VbFootstepConfig.generated.h"

USTRUCT()
struct FVbFootstepType
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    USoundBase* WalkFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* WalkStopFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* JogFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* JogStopFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* RunFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* RunStopFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* JumpFootstepCue = nullptr;

    UPROPERTY(EditDefaultsOnly)
    USoundBase* LandFootstepCue = nullptr;
};

UCLASS()
class UVbFootstepConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    FORCEINLINE const TMap<TEnumAsByte<EPhysicalSurface>, FVbFootstepType>& GetFootstepSoundMap() const { return FootstepSoundMap; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Config")
    TMap<TEnumAsByte<EPhysicalSurface>, FVbFootstepType> FootstepSoundMap;
};