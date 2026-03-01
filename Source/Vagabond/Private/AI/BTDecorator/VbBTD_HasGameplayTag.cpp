// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/BTDecorator/VbBTD_HasGameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UVbBTD_HasGameplayTag::UVbBTD_HasGameplayTag()
{
    ActorToCheckKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UVbBTD_HasGameplayTag, ActorToCheckKey), AActor::StaticClass());
}

bool UVbBTD_HasGameplayTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComponent) return false;

    AActor* ActorToCheck = Cast<AActor>(BlackboardComponent->GetValueAsObject(ActorToCheckKey.SelectedKeyName));
    if (!ActorToCheck) return false;

    const UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToCheck);
    if (!OwnerASC) return false;

    if (!OwnerASC->HasAnyMatchingGameplayTags(TagsToCheck)) return false;

    return true;
}