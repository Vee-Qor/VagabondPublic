// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbCoreTypes.h"
#include "Components/ActorComponent.h"
#include "VbCombatComponent.generated.h"

class UAbilitySystemComponent;
class AVbCharacter;
class UVbAbilitySystemComponent;
class UVbStyleData;
class UGameplayEffect;
class AVbWeapon;
class UVbImpactConfig;
class IGenericTeamAgentInterface;
struct FGameplayTag;
struct FGameplayEventData;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAttackHitDetected, const FHitResult& /*HitResult*/, const FVbAttackContext& /*AttackContext*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponChangedDelegate, const AVbWeapon* Weapon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVbCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVbCombatComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void Initialize(UVbAbilitySystemComponent* ASC, const TArray<UVbStyleData*>& DefaultStyles);
    void SwitchWeapon();
    void SetArmedState(bool bArmed) const;

    void SetWeaponStyle(UVbStyleData* NewStyle);
    void RemoveWeaponStyle(UVbStyleData* StyleToRemove);

    // ==== Attack Trace ==== // 
    bool DoSphereTraceStep(const FVbTraceParams& Params, TArray<FHitResult>& OutHitResults, bool& bDidTraceStep) const;
    bool DoBoxTraceStep(const FVbTraceParams& Params, TArray<FHitResult>& OutHitResults, bool& bDidTraceStep) const;
    void CollectUniqueEnemyHits(const TArray<FHitResult>& HitResults, const IGenericTeamAgentInterface* OwnerTeamAgent, TArray<FHitResult>& OutHitResults,
        TSet<AActor*>* AlreadyHitActors) const;
    void StartAreaTrace(const FVbAreaTraceInfo& InAreaTraceInfo);

    FOnAttackHitDetected OnAttackHitDetected;
    FOnCurrentWeaponChangedDelegate OnCurrentWeaponChanged;

    // ==== Accessors ==== //
    FORCEINLINE AVbWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

private:
    UFUNCTION()
    void OnRep_CurrentWeapon(AVbWeapon* OldWeapon);

    void OnArmedStateTagChanged(const FGameplayTag Tag, int32 NewCount);

    void SpawnAndSetWeapon(UVbStyleData* Style);
    bool TrySpawnNextStyleSet();
    int32 GetCurrentWeaponIndex() const;
    bool IsArmedState() const;
    void UpdateWeaponState() const;
    TSubclassOf<UGameplayEffect> GetDamageEffectClass() const;
    void SendBlockedHitEvent(AActor* HitActor) const;
    bool IsInFrontArc(const AActor* Target, const FVector& SourceLocation, float AngleDegrees = 180.0f) const;
    bool ShouldIgnoreHit(const UAbilitySystemComponent* SourceASC, const UAbilitySystemComponent* TargetASC) const;
    bool CanApplyDamageAfterDefense(AActor* HitActor, const UAbilitySystemComponent* SourceASC, const UAbilitySystemComponent* TargetASC,
        const FVector& InstigatorLocation, float& InOutDamage, bool& bOutWasBlocked) const;

    // ==== Attack Trace ==== // 
    void ApplyEffectOnHit(const FHitResult& HitResult, const FVbAttackContext& AttackContext) const;
    void StepAreaTrace();

    UPROPERTY(EditDefaultsOnly, Category="Damage")
    float BlockDamageReduction = 0.50f;

    // ==== Runtime ==== //
    FVbAreaTraceInfo AreaTraceInfo;

    UPROPERTY(Replicated)
    TArray<UVbStyleData*> StyleSet;

    UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon, BlueprintReadOnly, Category="Combat|Weapon", meta=(AllowPrivateAccess="true"))
    AVbWeapon* CurrentWeapon;

    UPROPERTY()
    UVbAbilitySystemComponent* AbilitySystemComponent;
};