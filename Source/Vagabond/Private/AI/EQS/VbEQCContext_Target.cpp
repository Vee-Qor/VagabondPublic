// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/EQS/VbEQCContext_Target.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void UVbEQCContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    Super::ProvideContext(QueryInstance, ContextData);

    AActor* Owner = Cast<AActor>(QueryInstance.Owner.Get());
    if (!Owner) return;

    AActor* TargetActor = nullptr;

    if (UBlackboardComponent* BlackboardComponent = UAIBlueprintHelperLibrary::GetBlackboard(Owner))
    {
        TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetKeyName));
    }

    if (!TargetActor)
    {
        TargetActor = UGameplayStatics::GetActorOfClass(Owner->GetWorld(), APlayerStart::StaticClass());
    }
  
    
    if (TargetActor)
    {
        UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
    }
}