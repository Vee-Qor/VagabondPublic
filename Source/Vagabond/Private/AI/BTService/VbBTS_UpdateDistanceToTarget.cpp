// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTService/VbBTS_UpdateDistanceToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UVbBTS_UpdateDistanceToTarget::UVbBTS_UpdateDistanceToTarget()
{
    NodeName = "UpdateDistanceToTarget";

    bCallTickOnSearchStart = true;

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTS_UpdateDistanceToTarget, TargetActorKey), AActor::StaticClass());
    DistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTS_UpdateDistanceToTarget, DistanceToTargetKey));
}

void UVbBTS_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    const AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return;

    const APawn* Pawn = Controller->GetPawn();
    if (!Pawn) return;

    UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent) return;

    const AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        BlackboardComponent->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, 0.0f);
        return;
    }

    BlackboardComponent->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Pawn->GetDistanceTo(TargetActor));
}