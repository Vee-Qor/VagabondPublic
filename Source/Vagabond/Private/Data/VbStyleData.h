// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbCoreTypes.h"
#include "Engine/DataAsset.h"
#include "VbStyleData.generated.h"

class AVbProjectile;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FVbWeaponVisual
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* Mesh = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName EquippedSocket = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName UnequippedSocket = NAME_None;

    bool IsValid() const { return Mesh && EquippedSocket != NAME_None && UnequippedSocket != NAME_None; }
};

USTRUCT(BlueprintType)
struct FVbWeaponVisualConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVbWeaponVisual MainHand;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVbWeaponVisual MainAccessoryHand;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVbWeaponVisual OffHand;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVbWeaponVisual OffAccessoryHand;
};

UCLASS()
class UVbStyleData : public UDataAsset
{
    GENERATED_BODY()

public:
    FORCEINLINE const FVbWeaponVisualConfig& GetVisualConfig() const { return VisualConfig; }
    FORCEINLINE const FGameplayTag& GetStyleTag() const { return StyleTag; }
    FORCEINLINE const FVbAbilityConfig& GetAbilityConfig() const { return AbilityConfig; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Visual")
    FVbWeaponVisualConfig VisualConfig;

    UPROPERTY(EditDefaultsOnly, Category="Identity", meta=(Categories="Combat.Style"))
    FGameplayTag StyleTag;

    UPROPERTY(EditDefaultsOnly, Category="Ability")
    FVbAbilityConfig AbilityConfig;
};