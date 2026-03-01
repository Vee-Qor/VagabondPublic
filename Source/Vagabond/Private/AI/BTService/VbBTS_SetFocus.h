// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "VbBTS_SetFocus.generated.h"

UCLASS()
class UVbBTS_SetFocus : public UBTService
{
    GENERATED_BODY()

public:
    UVbBTS_SetFocus();
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, Category="Keys")
    FBlackboardKeySelector TargetActorKey;
};