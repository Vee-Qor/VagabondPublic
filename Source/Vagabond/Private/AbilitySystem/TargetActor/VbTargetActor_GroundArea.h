// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "VbTargetActor_GroundArea.generated.h"

UCLASS()
class AVbTargetActor_GroundArea : public AGameplayAbilityTargetActor
{
    GENERATED_BODY()

public:
    AVbTargetActor_GroundArea();
    virtual void ConfirmTargetingAndContinue() override;

    void SetTargetAreaRadius(float AreaRadius);

protected:
    virtual void Tick(float DeltaSeconds) override;

private:
    FVector GetTargetPoint() const;

    UPROPERTY(EditDefaultsOnly, Category="Targeting")
    float TargetTraceRange = 2000.0f;

    UPROPERTY(EditDefaultsOnly, Category="Targeting")
    float TargetAreaRadius = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category="Targeting")
    UDecalComponent* DecalComponent;
};