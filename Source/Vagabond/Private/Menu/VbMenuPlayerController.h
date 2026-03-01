// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VbMenuPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateReadyDelegate, APlayerState* /*PlayerState*/);

UCLASS()
class AVbMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AVbMenuPlayerController();

    FOnPlayerStateReadyDelegate OnPlayerStateReady;

protected:
    virtual void BeginPlay() override;
    virtual void OnRep_PlayerState() override;

private:
    void SetViewToMenuCamera();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Menu")
    FName MenuCameraTag = "MenuCamera";
};