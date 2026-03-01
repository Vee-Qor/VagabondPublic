// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "VbBTD_HasGameplayTag.generated.h"

UCLASS()
class UVbBTD_HasGameplayTag : public UBTDecorator
{
    GENERATED_BODY()

public:
    UVbBTD_HasGameplayTag();
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
    
private:
    UPROPERTY(EditAnywhere)
    FBlackboardKeySelector ActorToCheckKey;
    
    UPROPERTY(EditAnywhere)
    FGameplayTagContainer TagsToCheck;
};