// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTService/VbBTS_CheckHealthPercent.h"

#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "VbGameplayTags.h"

UVbBTS_CheckHealthPercent::UVbBTS_CheckHealthPercent()
{
    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    bCreateNodeInstance = true;
}

void UVbBTS_CheckHealthPercent::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    if (const AAIController* AIController = OwnerComp.GetAIOwner())
    {
        if (APawn* Pawn = AIController->GetPawn())
        {
            OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
            if (OwnerASC.IsValid())
            {
                OwnerASC->AddLooseGameplayTag(VbGameplayTags::AI_State_Phase_One);
                OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetHealthAttribute()).AddUObject(this,
                    &UVbBTS_CheckHealthPercent::HealthChange, &OwnerComp);
            }
        }
    }
}

void UVbBTS_CheckHealthPercent::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (OwnerASC.IsValid())
    {
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetHealthAttribute()).RemoveAll(this);
    }

    Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UVbBTS_CheckHealthPercent::HealthChange(const FOnAttributeChangeData& ChangeData, UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp && OwnerASC.IsValid())
    {
        bool bFound = false;
        const float MaxHealth = OwnerASC->GetGameplayAttributeValue(UVbAttributeSet::GetMaxHealthAttribute(), bFound);

        if (bFound)
        {
            const float CurrentPercent = ChangeData.NewValue / MaxHealth;

            if (CurrentPercent <= PhaseThreeHealthPercent)
            {
                if (!OwnerASC->HasMatchingGameplayTag(VbGameplayTags::AI_State_Phase_Three))
                {
                    OwnerASC->AddLooseGameplayTag(VbGameplayTags::AI_State_Phase_Three);
                    OwnerASC->RemoveLooseGameplayTag(VbGameplayTags::AI_State_Phase_Two);
                    OwnerASC->RemoveLooseGameplayTag(VbGameplayTags::AI_State_Phase_One);
                }
            }
            else if (CurrentPercent <= PhaseTwoHealthPercent)
            {
                if (!OwnerASC->HasMatchingGameplayTag(VbGameplayTags::AI_State_Phase_Two))
                {
                    OwnerASC->AddLooseGameplayTag(VbGameplayTags::AI_State_Phase_Two);
                    OwnerASC->RemoveLooseGameplayTag(VbGameplayTags::AI_State_Phase_One);
                }
            }
        }
    }
}