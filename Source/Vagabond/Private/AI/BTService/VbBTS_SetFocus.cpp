// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTService/VbBTS_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UVbBTS_SetFocus::UVbBTS_SetFocus()
{
    NodeName = "Set Focus";

    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    bNotifyTick = false;

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTS_SetFocus, TargetActorKey), AActor::StaticClass());
}

void UVbBTS_SetFocus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    if (const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
    {
        AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
        if (!TargetActor) return;

        AIController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
    }
}

void UVbBTS_SetFocus::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnCeaseRelevant(OwnerComp, NodeMemory);

    if (AAIController* AIController = OwnerComp.GetAIOwner())
    {
        AIController->ClearFocus(EAIFocusPriority::Gameplay);
    }
}