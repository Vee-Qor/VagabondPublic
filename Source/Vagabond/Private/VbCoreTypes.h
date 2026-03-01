// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "VbCoreTypes.generated.h"

#define ECC_MeleeTrace ECollisionChannel::ECC_GameTraceChannel1

class UVbGameplayAbility;
class UVbStyleData;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FVbAbilityConfig
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UVbGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UVbGameplayAbility>> PassiveAbilities;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAnimInstance> AnimLayerClass;
};

USTRUCT(BlueprintType)
struct FVbHitReactMontages
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* Front = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* Back = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* Left = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* Right = nullptr;
};

struct FVbTraceParams
{
    FVector TraceStart = FVector::ZeroVector;
    FVector TraceEnd = FVector::ZeroVector;

    float DistanceBetweenTraces = 0.0f;
    bool bDrawDebug = false;

    float SphereRadius = 0.0f;
    FVector BoxSize = FVector::ZeroVector;
    FRotator BoxRotation = FRotator::ZeroRotator;

    AActor* AdditionalIgnoreActor = nullptr;
    TSet<AActor*>* AlreadyHitActors = nullptr;
};

UCLASS()
class UVbAttackTracePayload : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FName SocketName = NAME_None;

    UPROPERTY()
    FVector BoxSize = FVector::ZeroVector;

    UPROPERTY()
    float SphereRadius = 0.0f;

    UPROPERTY()
    float DistanceBetweenTraces = 0.0f;

    UPROPERTY()
    float Interval = 0.0f;

    UPROPERTY()
    bool bDrawDebug = false;

    UPROPERTY()
    FGameplayTag ImpactTypeTag;
};

class UVbItemInstance;

UCLASS()
class UVbSlotObject : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    int32 SlotIndex = INDEX_NONE;

    UPROPERTY()
    UVbItemInstance* ItemInstance = nullptr;
};

USTRUCT()
struct FVbAttackContext
{
    GENERATED_BODY()

    UPROPERTY()
    FGameplayTag ImpactTypeTag;

    UPROPERTY()
    FGameplayTag StyleTag;

    UPROPERTY()
    FGameplayTag OnHitEventTag;

    UPROPERTY()
    float Damage = 0.0f;
};

USTRUCT()
struct FVbAreaTraceInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    int32 TraceCount = 10;

    UPROPERTY(EditDefaultsOnly)
    float HitInterval = 0.1;

    UPROPERTY(EditDefaultsOnly)
    float Radius = 350.0f;

    UPROPERTY(EditDefaultsOnly)
    bool bDrawDebug = false;

    UPROPERTY()
    int32 TraceIndex = 0;

    UPROPERTY()
    FVector TraceStart = FVector::ZeroVector;

    UPROPERTY()
    FVector TraceEnd = FVector::ZeroVector;

    UPROPERTY()
    bool bFollowOwner = false;

    UPROPERTY()
    FTimerHandle TimerHandle;

    UPROPERTY()
    FVbAttackContext AttackContext;
};

USTRUCT(BlueprintType)
struct FVbAbilityParamsRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UVbGameplayAbility> AbilityClass;

    UPROPERTY(EditDefaultsOnly)
    FScalableFloat Damage = 0.0f;

    UPROPERTY(EditDefaultsOnly)
    FScalableFloat Cost = 0.0f;

    UPROPERTY(EditDefaultsOnly)
    FScalableFloat Cooldown = 0.0f;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag CooldownTag;
};

USTRUCT(BlueprintType)
struct FVbKeyIconRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FKey Key;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UTexture2D> Texture = nullptr;
};

UENUM()
enum class EVbTeam : uint8 { None, Player, Enemy };