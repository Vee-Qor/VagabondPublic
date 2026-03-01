// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "VbBTS_UpdateMotionWarping.generated.h"


UCLASS()
class UVbBTS_UpdateMotionWarping : public UBTService
{
    GENERATED_BODY()

public:
    UVbBTS_UpdateMotionWarping();
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, Category="MotionWarping")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category="MotionWarping")
    FName AttackTargetWarpName = "AttackTarget";
};