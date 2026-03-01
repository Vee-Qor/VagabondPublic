// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VbCharacterStats.generated.h"

USTRUCT()
struct FVbDefaultStatsValue
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    float MaxHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly)
    float MaxResource = 100.0f;

    UPROPERTY(EditDefaultsOnly)
    float MaxPoise = 100.0f;

    UPROPERTY(EditDefaultsOnly)
    float AttackPower = 1.0f;

    UPROPERTY(EditDefaultsOnly)
    float CriticalChance = 0.05f;

    UPROPERTY(EditDefaultsOnly)
    float CriticalMultiplier = 1.5f;

    UPROPERTY(EditDefaultsOnly)
    float Armor = 10.0f;

    UPROPERTY(EditDefaultsOnly)
    int32 MaxLevel = 10;

    UPROPERTY(EditDefaultsOnly)
    float MaxExperience = 0.0f;
};

UCLASS()
class UVbCharacterStats : public UDataAsset
{
    GENERATED_BODY()

public:
    FORCEINLINE const FVbDefaultStatsValue& GetDefault() const { return Default; }

private:
    UPROPERTY(EditDefaultsOnly)
    FVbDefaultStatsValue Default;
};