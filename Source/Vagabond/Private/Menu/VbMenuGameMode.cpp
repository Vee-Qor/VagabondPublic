// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/VbMenuGameMode.h"

#include "VbMenuHUD.h"
#include "VbMenuGameState.h"
#include "VbMenuPlayerController.h"
#include "Player/VbPlayerState.h"

AVbMenuGameMode::AVbMenuGameMode()
{
    PlayerControllerClass = AVbMenuPlayerController::StaticClass();
    GameStateClass = AVbMenuGameState::StaticClass();
    HUDClass = AVbMenuHUD::StaticClass();
    PlayerStateClass = AVbPlayerState::StaticClass();
    DefaultPawnClass = nullptr;
    bUseSeamlessTravel = true;
}