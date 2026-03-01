// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"
#include "VbAIController.generated.h"

class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UVbEnemyData;
class UGameplayEffect;
struct FAIStimulus;

UCLASS()
class AVbAIController : public AAIController
{
    GENERATED_BODY()

public:
    AVbAIController();
    virtual void OnPossess(APawn* InPawn) override;
    virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

    void SetAllSenseEnabled(bool bEnable) const;

private:
    void SelectTarget();
    void ClearTarget();
    void SetTarget(AActor* TargetActor);
    void SetTargetToAim(AActor* TargetActor) const;
    void SetOverlayEnabled(bool bEnabled, const AActor* TargetActor) const;

    // ==== Utils ==== //
    AActor* GetCurrentTarget() const;
    bool IsTargetDead(AActor* TargetActor) const;

    UFUNCTION()
    void OnEnemyPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

    UFUNCTION()
    void OnEnemyPerceptionForgotten(AActor* ForgotActor);

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UAIPerceptionComponent* EnemyPerceptionComponent;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UAISenseConfig_Sight* AISenseConfig_Sight;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UAISenseConfig_Hearing* AISenseConfig_Hearing;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UAISenseConfig_Damage* AISenseConfig_Damage;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    FName TargetActorKeyName = "TargetActor";

    UPROPERTY(EditDefaultsOnly, Category="AI")
    FName MeleeAttackDistanceKeyName = "MeleeAttackDistance";

    UPROPERTY(EditDefaultsOnly, Category="AI")
    FName RangeAttackDistanceKeyName = "RangeAttackDistance";

    UPROPERTY(EditDefaultsOnly, Category="AI")
    FName StrafeRangeKeyName = "StrafeRange";

    UPROPERTY(EditDefaultsOnly, Category="AI")
    FName ChasingRangeKeyName = "ChasingRange";
};