// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "VbInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FVbInputMappingConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta=(Categories="Input"))
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly)
    UInputAction* InputAction = nullptr;

    bool IsValid() const { return InputTag.IsValid() && InputAction; }
};

UCLASS()
class UVbInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Input")
    const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;

    UFUNCTION(BlueprintPure, Category="Input")
    FORCEINLINE UInputMappingContext* GetDefaultMappingContext() const { return DefaultMappingContext; }

    UFUNCTION(BlueprintPure, Category="Input")
    FORCEINLINE UInputMappingContext* GetCombatMappingContext() const { return CombatMappingContext; }

    UFUNCTION(BlueprintPure, Category="Input")
    FORCEINLINE TArray<FVbInputMappingConfig> GetAbilityInputMappings() const { return AbilityInputMappings; }
    
    UFUNCTION(BlueprintPure, Category="Input")
    FORCEINLINE TArray<FVbInputMappingConfig> GetNativeInputMappings() const { return NativeInputMappings; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputMappingContext* CombatMappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input", meta=(TitleProperty="InputTag"))
    TArray<FVbInputMappingConfig> NativeInputMappings;

    UPROPERTY(EditDefaultsOnly, Category="Input", meta=(TitleProperty="InputTag"))
    TArray<FVbInputMappingConfig> AbilityInputMappings;
};