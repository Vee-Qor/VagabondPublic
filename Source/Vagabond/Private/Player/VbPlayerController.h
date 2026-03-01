// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "VbPlayerController.generated.h"

class UAbilitySystemComponent;

UCLASS()
class AVbPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; }
    virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

    UFUNCTION(Client, Reliable)
    void Client_InitOnScreenOverlay(UAbilitySystemComponent* EnemyASC);

    UFUNCTION(Client, Reliable)
    void Client_DisableOnScreenOverlay();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override;

private:
    FGenericTeamId TeamID;
};