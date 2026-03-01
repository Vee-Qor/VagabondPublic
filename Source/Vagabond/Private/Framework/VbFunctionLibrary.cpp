// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbFunctionLibrary.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "Character/VbCharacter.h"
#include "Data/VbCharacterData.h"
#include "Data/Player/VbPlayerData.h"
#include "VbGameInstance.h"
#include "GameplayEffectTypes.h"
#include "InputMappingContext.h"
#include "VbImpactConfig.h"
#include "Input/VbInputConfig.h"

const FVbAbilityParamsRow* UVbFunctionLibrary::GetAbilityParamsRow(const UObject* WorldContextObject, const TSubclassOf<UGameplayAbility>& AbilityClass)
{
    if (!WorldContextObject || !AbilityClass) return nullptr;

    const UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    const UVbGameInstance* GameInstance = World->GetGameInstance<UVbGameInstance>();
    if (!GameInstance) return nullptr;

    const UDataTable* DataTable = GameInstance->GetAbilityParamsRow();
    if (!DataTable) return nullptr;

    TArray<FVbAbilityParamsRow*> Rows;
    DataTable->GetAllRows("", Rows);

    for (const FVbAbilityParamsRow* Row : Rows)
    {
        if (!Row || Row->AbilityClass != AbilityClass) continue;

        return Row;
    }

    return nullptr;
}

FScalableFloat UVbFunctionLibrary::GetAbilityDamage(const UObject* WorldContextObject, const TSubclassOf<UGameplayAbility>& AbilityClass)
{
    if (!WorldContextObject || !AbilityClass) return 0.0f;
    const FVbAbilityParamsRow* AbilityParamsRow = GetAbilityParamsRow(WorldContextObject, AbilityClass);

    return AbilityParamsRow ? AbilityParamsRow->Damage : 0.0f;
}

const FKey* UVbFunctionLibrary::GetKeyForInputTag(AActor* Owner, const FGameplayTag& InputTag)
{
    if (!Owner) return nullptr;

    const UWorld* World = Owner->GetWorld();
    if (!World) return nullptr;

    const UVbInputConfig* InputConfig = GetInputConfig(Owner);
    if (!InputConfig) return nullptr;

    const UInputMappingContext* CombatMappingContext = InputConfig->GetCombatMappingContext();
    if (!CombatMappingContext) return nullptr;

    const UInputMappingContext* DefaultMappingContext = InputConfig->GetDefaultMappingContext();
    if (!DefaultMappingContext) return nullptr;

    const TArray<FVbInputMappingConfig>& AbilityMappings = InputConfig->GetAbilityInputMappings();
    const TArray<FVbInputMappingConfig>& NativeMappings = InputConfig->GetNativeInputMappings();

    for (const FVbInputMappingConfig& Config : AbilityMappings)
    {
        if (Config.InputTag != InputTag) continue;

        for (const FEnhancedActionKeyMapping& Mapping : CombatMappingContext->GetMappings())
        {
            if (Mapping.Action != Config.InputAction) continue;

            return &Mapping.Key;
        }
    }

    for (const FVbInputMappingConfig& Config : AbilityMappings)
    {
        if (Config.InputTag != InputTag) continue;

        for (const FEnhancedActionKeyMapping& Mapping : DefaultMappingContext->GetMappings())
        {
            if (Mapping.Action != Config.InputAction) continue;

            return &Mapping.Key;
        }
    }

    for (const FVbInputMappingConfig& Config : NativeMappings)
    {
        if (Config.InputTag != InputTag) continue;

        for (const FEnhancedActionKeyMapping& Mapping : CombatMappingContext->GetMappings())
        {
            if (Mapping.Action != Config.InputAction) continue;

            return &Mapping.Key;
        }
    }

    for (const FVbInputMappingConfig& Config : NativeMappings)
    {
        if (Config.InputTag != InputTag) continue;

        for (const FEnhancedActionKeyMapping& Mapping : DefaultMappingContext->GetMappings())
        {
            if (Mapping.Action != Config.InputAction) continue;

            return &Mapping.Key;
        }
    }

    return nullptr;
}

const UDataTable* UVbFunctionLibrary::GetKeyIconRow(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    const UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    const UVbGameInstance* GameInstance = World->GetGameInstance<UVbGameInstance>();
    if (!GameInstance) return nullptr;

    return GameInstance->GetKeyIconRow();
}

UVbImpactConfig* UVbFunctionLibrary::GetImpactConfig(const UObject* WorldContextObject)
{
    if (WorldContextObject)
    {
        const UWorld* World = WorldContextObject->GetWorld();
        if (!World) return nullptr;

        const UVbGameInstance* GameInstance = World->GetGameInstance<UVbGameInstance>();
        return GameInstance ? GameInstance->GetImpactConfig() : nullptr;
    }

    return nullptr;
}

EPhysicalSurface UVbFunctionLibrary::GetSurfaceFromCue(const FGameplayCueParameters& CueParams)
{
    if (CueParams.PhysicalMaterial.IsValid())
    {
        return CueParams.PhysicalMaterial->SurfaceType;
    }

    return SurfaceType_Default;
}

bool UVbFunctionLibrary::ResolveImpactFromCue(const UObject* WorldContextObject, const FGameplayCueParameters& CueParams, FVbImpact& OutImpact)
{
    OutImpact = FVbImpact{};

    const UVbImpactConfig* Config = GetImpactConfig(WorldContextObject);
    if (!Config || !CueParams.OriginalTag.IsValid()) return false;

    const EPhysicalSurface Surface = GetSurfaceFromCue(CueParams);

    Config->ResolveImpact(CueParams.OriginalTag, CueParams.AggregatedSourceTags, Surface, OutImpact);

    return !OutImpact.Sounds.IsEmpty() || !OutImpact.NiagaraSystems.IsEmpty() || !OutImpact.DecalData.IsEmpty();
}

const UVbInputConfig* UVbFunctionLibrary::GetInputConfig(AActor* Owner)
{
    const AVbCharacter* Character = Cast<AVbCharacter>(Owner);
    if (!Character) return nullptr;

    const UVbPlayerData* PlayerData = Cast<UVbPlayerData>(Character->GetCharacterData());
    if (!PlayerData) return nullptr;

    return PlayerData ? PlayerData->GetInputConfig() : nullptr;
}