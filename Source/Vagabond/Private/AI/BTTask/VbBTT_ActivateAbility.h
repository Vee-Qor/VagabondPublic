// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "VbBTT_ActivateAbility.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class UVbBTT_ActivateAbility : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UVbBTT_ActivateAbility();
    virtual FString GetStaticDescription() const override;
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& InOwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& InOwnerComp, uint8* NodeMemory) override;

private:
    void AbilityEnd(UGameplayAbility* Ability);
    void Cleanup();

    UPROPERTY(EditAnywhere, Category="Ability")
    FGameplayTag AbilityTag;

    UPROPERTY()
    TWeakObjectPtr<UBehaviorTreeComponent> OwnerComp;

    UPROPERTY()
    TWeakObjectPtr<UAbilitySystemComponent> SourceASC;
};