// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#include "CoreMinimal.h"
#include "VbBTD_CompareBlackboardFloats.generated.h"

UCLASS()
class UVbBTD_CompareBlackboardFloats : public UBTDecorator
{
    GENERATED_BODY()

public:
    UVbBTD_CompareBlackboardFloats();
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
    EBlackboardNotificationResult OnBlackboardChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey KeyID);

    UPROPERTY(EditAnywhere, Category="Keys")
    FBlackboardKeySelector ValueOneKey;

    UPROPERTY(EditAnywhere, Category = Condition)
    TEnumAsByte<EArithmeticKeyOperation::Type> ArithmeticOperation;

    UPROPERTY(EditAnywhere, Category="Keys")
    FBlackboardKeySelector ValueTwoKey;
};