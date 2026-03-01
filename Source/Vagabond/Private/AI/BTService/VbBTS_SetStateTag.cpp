// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTService/VbBTS_SetStateTag.h"

#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UVbBTS_SetStateTag::UVbBTS_SetStateTag()
{
    NodeName = "SetStateTag";

    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    bNotifyTick = false;
}

FString UVbBTS_SetStateTag::GetStaticDescription() const
{
    if (StateTag.IsValid())
    {
        return FString::Printf(TEXT("Set %s Tag"), *StateTag.ToString());
    }

    return TEXT("Set None Tag");
}

void UVbBTS_SetStateTag::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    const AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return;

    if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
    {
        SourceASC->AddLooseGameplayTag(StateTag);
    }
}

void UVbBTS_SetStateTag::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnCeaseRelevant(OwnerComp, NodeMemory);

    const AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return;

    if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
    {
        SourceASC->RemoveLooseGameplayTag(StateTag);
    }
}