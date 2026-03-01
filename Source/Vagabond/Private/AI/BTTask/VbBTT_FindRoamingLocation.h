// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "VbBTT_FindRoamingLocation.generated.h"

UCLASS()
class UVbBTT_FindRoamingLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UVbBTT_FindRoamingLocation();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere)
    float Radius = 2500.0f;

    UPROPERTY(EditAnywhere, Category="Keys")
    FBlackboardKeySelector RoamingLocationKey;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, Category="Debug", meta=(EditCondition="bDrawDebug"))
    float LifeTime = 5.0f;
};