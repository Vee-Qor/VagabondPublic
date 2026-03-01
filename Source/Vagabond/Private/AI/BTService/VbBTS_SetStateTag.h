// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"
#include "VbBTS_SetStateTag.generated.h"

UCLASS()
class UVbBTS_SetStateTag : public UBTService
{
    GENERATED_BODY()

public:
    UVbBTS_SetStateTag();
    virtual FString GetStaticDescription() const override;
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, Category="Tag")
    FGameplayTag StateTag;
};