// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTDecorator/VbBTD_CompareBlackboardFloats.h"

#include "BehaviorTree/BlackboardComponent.h"

UVbBTD_CompareBlackboardFloats::UVbBTD_CompareBlackboardFloats()
{
    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
}

void UVbBTD_CompareBlackboardFloats::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    
    UBlackboardData* BBAsset = GetBlackboardAsset();
    if (BBAsset)
    {
        ValueOneKey.ResolveSelectedKey(*BBAsset);
        ValueTwoKey.ResolveSelectedKey(*BBAsset);
    }
}

void UVbBTD_CompareBlackboardFloats::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
    {
        BlackboardComponent->RegisterObserver(ValueOneKey.GetSelectedKeyID(), this,
            FOnBlackboardChangeNotification::CreateUObject(this, &UVbBTD_CompareBlackboardFloats::OnBlackboardChange));
        BlackboardComponent->RegisterObserver(ValueTwoKey.GetSelectedKeyID(), this,
            FOnBlackboardChangeNotification::CreateUObject(this, &UVbBTD_CompareBlackboardFloats::OnBlackboardChange));
    }
}

void UVbBTD_CompareBlackboardFloats::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
    {
        BlackboardComponent->UnregisterObserversFrom(this);
    }
    Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

FString UVbBTD_CompareBlackboardFloats::GetStaticDescription() const
{
    FString OperationStr;
    switch (ArithmeticOperation)
    {
        case EArithmeticKeyOperation::Equal: OperationStr = TEXT("==");
            break;
        case EArithmeticKeyOperation::NotEqual: OperationStr = TEXT("!=");
            break;
        case EArithmeticKeyOperation::Less: OperationStr = TEXT("<");
            break;
        case EArithmeticKeyOperation::LessOrEqual: OperationStr = TEXT("<=");
            break;
        case EArithmeticKeyOperation::Greater: OperationStr = TEXT(">");
            break;
        case EArithmeticKeyOperation::GreaterOrEqual: OperationStr = TEXT(">=");
            break;
        default: OperationStr = TEXT("?");
            break;
    }

    return FString::Printf(TEXT("%s %s %s"),
        *ValueOneKey.SelectedKeyName.ToString(),
        *OperationStr,
        *ValueTwoKey.SelectedKeyName.ToString());
}

bool UVbBTD_CompareBlackboardFloats::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
    {
        const float ValueOne = BlackboardComponent->GetValueAsFloat(ValueOneKey.SelectedKeyName);
        const float ValueTwo = BlackboardComponent->GetValueAsFloat(ValueTwoKey.SelectedKeyName);

        switch (ArithmeticOperation)
        {
            case EArithmeticKeyOperation::Equal: return ValueOne == ValueTwo;
            case EArithmeticKeyOperation::NotEqual: return ValueOne != ValueTwo;
            case EArithmeticKeyOperation::Less: return ValueOne < ValueTwo;
            case EArithmeticKeyOperation::LessOrEqual: return ValueOne <= ValueTwo;
            case EArithmeticKeyOperation::Greater: return ValueOne > ValueTwo;
            case EArithmeticKeyOperation::GreaterOrEqual: return ValueOne >= ValueTwo;
            default: return false;
        }
    }

    return false;
}

EBlackboardNotificationResult UVbBTD_CompareBlackboardFloats::OnBlackboardChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey KeyID)
{
    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
    
    if (OwnerComp)
    {
        ConditionalFlowAbort(*OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
    }
    
    return EBlackboardNotificationResult::ContinueObserving;
}