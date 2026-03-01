// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTTask/VbBTT_ActivateAbility.h"

#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "BehaviorTree/BlackboardComponent.h"

UVbBTT_ActivateAbility::UVbBTT_ActivateAbility()
{
    NodeName = "ActivateAbility";
    bCreateNodeInstance = true;
}

FString UVbBTT_ActivateAbility::GetStaticDescription() const
{
    if (AbilityTag.IsValid())
    {
        return AbilityTag.ToString();
    }

    return "None";
}

EBTNodeResult::Type UVbBTT_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* NodeMemory)
{
    const AAIController* AIController = InOwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
    if (!SourceASC.IsValid()) return EBTNodeResult::Failed;

    const UBlackboardComponent* BlackboardComponent = InOwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent) return EBTNodeResult::Failed;

    if (!AbilityTag.IsValid()) return EBTNodeResult::Failed;

    SourceASC->AbilityEndedCallbacks.AddUObject(this, &UVbBTT_ActivateAbility::AbilityEnd);

    if (!SourceASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer()))
    {
        Cleanup();
        return EBTNodeResult::Failed;
    }

    OwnerComp = &InOwnerComp;
    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UVbBTT_ActivateAbility::AbortTask(UBehaviorTreeComponent& InOwnerComp, uint8* NodeMemory)
{
    Cleanup();
    return EBTNodeResult::Aborted;
}

void UVbBTT_ActivateAbility::AbilityEnd(UGameplayAbility* Ability)
{
    if (!OwnerComp.IsValid() || !Ability || !Ability->GetAssetTags().HasTagExact(AbilityTag)) return;

    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    Cleanup();
}

void UVbBTT_ActivateAbility::Cleanup()
{
    if (SourceASC.IsValid())
    {
        SourceASC->AbilityEndedCallbacks.RemoveAll(this);
    }

    OwnerComp.Reset();
    SourceASC.Reset();
}