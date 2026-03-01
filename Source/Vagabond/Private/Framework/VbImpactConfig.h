// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "VbImpactConfig.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FVbDecalData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UMaterialInterface* Decal = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector Size = FVector(10.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Lifetime = 2.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FadeOutTime = 0.7f;
};

USTRUCT(BlueprintType)
struct FVbImpact
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<USoundBase*> Sounds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<UNiagaraSystem*> NiagaraSystems;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FVbDecalData> DecalData;
};

USTRUCT(BlueprintType)
struct FVbImpactRule : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Impact"))
    FGameplayTag EventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTagContainer RequiredTags;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTagContainer BlockedTags;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TEnumAsByte<EPhysicalSurface> Surface = SurfaceType_Default;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVbImpact Effects;
};

UCLASS()
class UVbImpactConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void ResolveImpact(const FGameplayTag& EventTag, const FGameplayTagContainer& ContextTags, const EPhysicalSurface& Surface, FVbImpact& OutImpact) const;

private:
    UPROPERTY(EditDefaultsOnly, Category="Impact")
    UDataTable* RulesTable;
};