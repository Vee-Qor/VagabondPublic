// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbCoreTypes.h"
#include "Engine/DataAsset.h"
#include "VbCharacterData.generated.h"

class AVbAIController;
class UBlackboardData;
class UBehaviorTree;
class UVbCharacterStats;
class UVbFootstepConfig;
class UVbStyleData;
class UVbInputConfig;
class UVbItemStatic;

USTRUCT()
struct FVbKnockdownAnimationSet
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* KnockdownStart = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* KnockdownLoop = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* KnockdownLand = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* KnockdownRecovery = nullptr;
};

UCLASS()
class UVbCharacterData : public UDataAsset
{
    GENERATED_BODY()

public:
    FORCEINLINE const FVbAbilityConfig& GetAbilityConfig() const { return AbilityConfig; }
    FORCEINLINE const TArray<UVbStyleData*>& GetDefaultStyles() const { return DefaultStyles; }
    FORCEINLINE const TArray<UAnimMontage*>& GetDeathMontages() const { return DeathMontages; }
    FORCEINLINE const TArray<UAnimMontage*>& GetRespawnMontages() const { return RespawnMontages; }
    FORCEINLINE UAnimMontage* GetGrabbedMontage() const { return GrabbedMontage; }
    FORCEINLINE UVbFootstepConfig* GetFootstepConfig() const { return FootstepConfig; }
    FORCEINLINE UVbCharacterStats* GetStats() const { return CharacterStats; }
    FORCEINLINE const TSubclassOf<UAnimInstance>& GetRigAnimLayerClass() const { return RigAnimLayerClass; }
    FORCEINLINE FVbHitReactMontages GetHitReactMontages() const { return HitReactMontages; }
    FORCEINLINE const FVbKnockdownAnimationSet& GetKnockdownAnimationSet() const { return KnockdownAnimationSet; }
    FORCEINLINE FText GetCharacterName() const { return CharacterName; }
    FORCEINLINE EVbTeam GetDefaultTeamID() const { return DefaultTeam; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Team")
    EVbTeam DefaultTeam = EVbTeam::None;

    UPROPERTY(EditDefaultsOnly, Category="Ability")
    FVbAbilityConfig AbilityConfig;

    UPROPERTY(EditDefaultsOnly, Category="Combat")
    TArray<UVbStyleData*> DefaultStyles;

    UPROPERTY(EditDefaultsOnly, Category="Combat")
    UVbCharacterStats* CharacterStats;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    TSubclassOf<UAnimInstance> RigAnimLayerClass;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    TArray<UAnimMontage*> DeathMontages;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    TArray<UAnimMontage*> RespawnMontages;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    UAnimMontage* GrabbedMontage;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FVbHitReactMontages HitReactMontages;

    UPROPERTY(EditDefaultsOnly, Category="Animation")
    FVbKnockdownAnimationSet KnockdownAnimationSet;

    UPROPERTY(EditDefaultsOnly, Category="Sound")
    UVbFootstepConfig* FootstepConfig;

    UPROPERTY(EditDefaultsOnly, Category="UI")
    FText CharacterName;
};