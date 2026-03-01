// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VbFunctionLibrary.generated.h"

struct FVbKeyIconRow;
struct FVbAbilityParamsRow;
class UAbilitySystemComponent;
class UGameplayAbility;
class UVbInputConfig;
class UVbImpactConfig;
struct FGameplayTag;
struct FGameplayCueParameters;
struct FVbImpact;

UCLASS()
class UVbFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static const FVbAbilityParamsRow* GetAbilityParamsRow(const UObject* WorldContextObject, const TSubclassOf<UGameplayAbility>& AbilityClass);
    static FScalableFloat GetAbilityDamage(const UObject* WorldContextObject, const TSubclassOf<UGameplayAbility>& AbilityClass);

    // ==== Impact Config ==== //
    UFUNCTION(BlueprintPure, Category="Impact", meta=(WorldContext="WorldContextObject"))
    static UVbImpactConfig* GetImpactConfig(const UObject* WorldContextObject);

    static EPhysicalSurface GetSurfaceFromCue(const FGameplayCueParameters& CueParams);

    UFUNCTION(BlueprintCallable, Category="Impact", meta=(WorldContext="WorldContextObject"))
    static bool ResolveImpactFromCue(const UObject* WorldContextObject, const FGameplayCueParameters& CueParams, FVbImpact& OutImpact);

    // ==== Input Config ==== //
    UFUNCTION(BlueprintCallable, Category="Impact")
    static const UVbInputConfig* GetInputConfig(AActor* Owner);
    static const FKey* GetKeyForInputTag(AActor* Owner, const FGameplayTag& InputTag);
    static const UDataTable* GetKeyIconRow(const UObject* WorldContextObject);
};