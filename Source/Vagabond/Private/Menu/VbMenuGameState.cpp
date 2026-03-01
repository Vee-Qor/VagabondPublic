// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/VbMenuGameState.h"

#include "Framework/VbGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Player/VbPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogVbGameState, All, All);

void AVbMenuGameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    if (HasAuthority())
    {
        AVbPlayerState* VbPlayerState = Cast<AVbPlayerState>(PlayerState);
        if (!VbPlayerState) return;

        VbPlayerState->OnReadyChanged.AddUObject(this, &AVbMenuGameState::OnReadyChanged);
    }

    OnSessionPlayersChanged.Broadcast();
}

void AVbMenuGameState::RemovePlayerState(APlayerState* PlayerState)
{
    if (HasAuthority())
    {
        if (AVbPlayerState* VbPlayerState = Cast<AVbPlayerState>(PlayerState))
        {
            VbPlayerState->OnReadyChanged.RemoveAll(this);
        }
    }

    Super::RemovePlayerState(PlayerState);

    ResetMatchStart();
    OnSessionPlayersChanged.Broadcast();
}

void AVbMenuGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbMenuGameState, MatchStartTimeServer);
}

void AVbMenuGameState::TryStartMatch()
{
    if (!HasAuthority()) return;

    if (!IsAllReady())
    {
        CancelMatchCountdown();
        return;
    }

    StartMatchCountdown();
}

void AVbMenuGameState::ResetMatchStart()
{
    if (!HasAuthority()) return;

    CancelMatchCountdown();

    for (APlayerState* State : PlayerArray)
    {
        AVbPlayerState* VbPlayerState = Cast<AVbPlayerState>(State);
        if (!VbPlayerState) continue;

        VbPlayerState->SetReady(false);
    }
}

void AVbMenuGameState::StartMatchCountdown()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    MatchStartTimeServer = GetServerWorldTimeSeconds() + MatchStartDelay;
    OnSessionStartTimer.Broadcast(true);

    UE_LOG(LogVbGameState, Warning, TEXT("Start Match Countdown | Delay: %.1f"), MatchStartDelay);
    World->GetTimerManager().ClearTimer(MatchStartTimerHandle);
    World->GetTimerManager().SetTimer(MatchStartTimerHandle, GameInstance, &UVbGameInstance::StartSession, MatchStartDelay);
}

void AVbMenuGameState::CancelMatchCountdown()
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(MatchStartTimerHandle);
        MatchStartTimeServer = 0.0f;
        OnSessionStartTimer.Broadcast(false);
    }
}

void AVbMenuGameState::OnReadyChanged(bool bReady)
{
    TryStartMatch();
}

void AVbMenuGameState::OnRep_MatchStartTimeServer()
{
    OnSessionStartTimer.Broadcast(MatchStartTimeServer > 0.0f);
}

bool AVbMenuGameState::IsAllReady() const
{
    UE_LOG(LogVbGameState, Warning, TEXT("Player Nums: %i"), PlayerArray.Num());

    if (PlayerArray.Num() <= 0) return false;

    for (const APlayerState* State : PlayerArray)
    {
        const AVbPlayerState* PlayerState = Cast<AVbPlayerState>(State);
        if (!PlayerState) return false;

        UE_LOG(LogVbGameState, Warning, TEXT("Player: %s | %s"), *PlayerState->GetPlayerName(), PlayerState->IsReady() ? TEXT("Ready") : TEXT("Not Ready"));
        if (!PlayerState->IsReady()) return false;
    }

    return true;
}