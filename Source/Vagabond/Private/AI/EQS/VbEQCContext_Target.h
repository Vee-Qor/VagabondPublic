// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "VbEQCContext_Target.generated.h"

UCLASS()
class UVbEQCContext_Target : public UEnvQueryContext
{
    GENERATED_BODY()

public:
    virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

private:
    UPROPERTY(EditAnywhere)
    FName TargetKeyName = "TargetActor";
};