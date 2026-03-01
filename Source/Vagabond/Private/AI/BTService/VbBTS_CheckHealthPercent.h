// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "VbBTS_CheckHealthPercent.generated.h"

class UAbilitySystemComponent;
struct FOnAttributeChangeData;

UCLASS()
class UVbBTS_CheckHealthPercent : public UBTService
{
    GENERATED_BODY()

public:
    UVbBTS_CheckHealthPercent();
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    void HealthChange(const FOnAttributeChangeData& ChangeData, UBehaviorTreeComponent* OwnerComp);

    UPROPERTY(EditAnywhere)
    float PhaseTwoHealthPercent = 0.75f;

    UPROPERTY(EditAnywhere)
    float PhaseThreeHealthPercent = 0.5f;

    UPROPERTY()
    TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
};