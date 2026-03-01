// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTTask/VbBTT_FindRoamingLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UVbBTT_FindRoamingLocation::UVbBTT_FindRoamingLocation()
{
    RoamingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTT_FindRoamingLocation, RoamingLocationKey));
}

EBTNodeResult::Type UVbBTT_FindRoamingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent) return EBTNodeResult::Failed;

    const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    FNavLocation NavLocation;
    if (NavigationSystem && NavigationSystem->GetRandomReachablePointInRadius(AIController->GetPawn()->GetActorLocation(), Radius, NavLocation))
    {
        BlackboardComponent->SetValueAsVector(RoamingLocationKey.SelectedKeyName, NavLocation.Location);

        if (bDrawDebug)
        {
            DrawDebugSphere(AIController->GetWorld(), AIController->GetPawn()->GetActorLocation(), Radius, 16, FColor::Blue, false, LifeTime);
            DrawDebugSphere(AIController->GetWorld(), NavLocation.Location, 50.0f, 16, FColor::Green, false, LifeTime);
        }

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}