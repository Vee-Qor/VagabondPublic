// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTService/VbBTS_UpdateMotionWarping.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/VbCharacter.h"
#include "MotionWarpingComponent.h"

UVbBTS_UpdateMotionWarping::UVbBTS_UpdateMotionWarping()
{
    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTS_UpdateMotionWarping, TargetActorKey), AActor::StaticClass());
}

void UVbBTS_UpdateMotionWarping::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    const AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    const AVbCharacter* Character = Cast<AVbCharacter>(AIController->GetPawn());
    if (!Character) return;

    UMotionWarpingComponent* MotionWarpingComponent = Character->GetMotionWarpingComponent();
    if (!MotionWarpingComponent) return;

    const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent) return;

    const AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor) return;

    MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(AttackTargetWarpName, TargetActor->GetActorLocation());
}