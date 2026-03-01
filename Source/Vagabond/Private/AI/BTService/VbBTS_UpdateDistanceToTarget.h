// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "VbBTS_UpdateDistanceToTarget.generated.h"

UCLASS()
class UVbBTS_UpdateDistanceToTarget : public UBTService
{
    GENERATED_BODY()

public:
    UVbBTS_UpdateDistanceToTarget();
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, Category="Target")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category="Target")
    FBlackboardKeySelector DistanceToTargetKey;
};