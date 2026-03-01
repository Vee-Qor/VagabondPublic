// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VbMenuHUD.generated.h"

class UVbMainMenuWidget;

UCLASS()
class AVbMenuHUD : public AHUD
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    void CreateMainMenuWidget() const;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UVbMainMenuWidget> MainMenuWidgetClass;
};