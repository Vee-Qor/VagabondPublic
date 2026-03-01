// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbGameMode.h"

#include "Character/VbPlayerCharacter.h"
#include "GameplayEffect.h"
#include "VbGameState.h"
#include "Player/VbPlayerController.h"
#include "Player/VbPlayerState.h"
#include "UI/HUD/VbGameplayHUD.h"

AVbGameMode::AVbGameMode()
{
    DefaultPawnClass = AVbPlayerCharacter::StaticClass();
    PlayerControllerClass = AVbPlayerController::StaticClass();
    PlayerStateClass = AVbPlayerState::StaticClass();
    HUDClass = AVbGameplayHUD::StaticClass();
    GameStateClass = AVbGameState::StaticClass();
}

void AVbGameMode::BeginPlay()
{
    Super::BeginPlay();

    ensure(ArmedStatusEffectClass);
    ensure(UnarmedStatusEffectClass);
    ensure(DeadEffectClass);
}