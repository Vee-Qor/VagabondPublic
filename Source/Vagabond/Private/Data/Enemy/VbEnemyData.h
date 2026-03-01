// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/VbCharacterData.h"
#include "VbEnemyData.generated.h"

UENUM()
enum class EVbEnemyType : uint8 { Rare, Elite, Boss };

class UUserWidget;

UCLASS()
class UVbEnemyData : public UVbCharacterData
{
    GENERATED_BODY()

public:
    FORCEINLINE EVbEnemyType GetEnemyType() const { return EnemyType; }
    FORCEINLINE TSubclassOf<AVbAIController> GetAIControllerClass() const { return AIControllerClass; }
    FORCEINLINE TSubclassOf<UUserWidget> GetEnemyOverlayWidgetClass() const { return EnemyOverlayWidgetClass; }
    FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
    FORCEINLINE UBlackboardData* GetBlackboardData() const { return BlackboardData; }
    FORCEINLINE float GetMeleeAttackDistance() const { return MeleeAttackDistance; }
    FORCEINLINE float GetRangeAttackDistance() const { return RangeAttackDistance; }
    FORCEINLINE float GetStrafeRange() const { return StrafeRange; }
    FORCEINLINE float GetChasingRange() const { return ChasingRange; }

private:
    UPROPERTY(EditDefaultsOnly, Category="AI")
    TSubclassOf<AVbAIController> AIControllerClass;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UBehaviorTree* BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    UBlackboardData* BlackboardData;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    TSubclassOf<UUserWidget> EnemyOverlayWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    float MeleeAttackDistance = 200.0f;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    float RangeAttackDistance = 400.0f;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    float StrafeRange = 500.0f;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    float ChasingRange = 700.0f;

    UPROPERTY(EditDefaultsOnly, Category="AI")
    EVbEnemyType EnemyType = EVbEnemyType::Rare;
};