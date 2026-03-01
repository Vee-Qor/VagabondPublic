// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VbFootstepComponent.generated.h"

class UVbFootstepConfig;
struct FVbFootstepType;

UENUM()
enum class EVbFoot : uint8 { Right, Left, Both };

UENUM()
enum class EVbFootstepMovement : uint8 { Walk, WalkStop, Jog, JogStop, Run, RunStop, Jump, Land };

USTRUCT()
struct FVbFootstepContext
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EVbFoot Foot = EVbFoot::Right;

    UPROPERTY(EditAnywhere)
    EVbFootstepMovement FootstepMovement = EVbFootstepMovement::Walk;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVbFootstepComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVbFootstepComponent();

    void HandleFootStep(const FVbFootstepContext& Context) const;

private:
    UVbFootstepConfig* GetFootstepConfig() const;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    FName RightFootSocketName = "foot_r";

    UPROPERTY(EditDefaultsOnly, Category="Config")
    FName LeftFootSocketName = "foot_l";

    UPROPERTY(EditDefaultsOnly, Category="Config")
    FName BothFootSocketName = "pelvis";

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float DefaultSocketOffset = 50.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    float BothSocketOffset = 150.0f;

    UPROPERTY(EditDefaultsOnly, Category="Config")
    bool bDrawDebug = false;
};