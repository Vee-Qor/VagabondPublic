// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbGameState.h"

#include "VbGameInstance.h"
#include "Net/UnrealNetwork.h"

void AVbGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbGameState, ExtractionEndTimeServer);
}

void AVbGameState::StartExtraction()
{
    if (!HasAuthority()) return;

    ExtractionEndTimeServer = GetServerWorldTimeSeconds() + ExtractionTimeDelay;

    const UWorld* World = GetWorld();
    if (!World) return;

    OnExtractionCountdownUpdated.Broadcast(true);
    World->GetTimerManager().SetTimer(ExtractionTimer, this, &AVbGameState::ExtractPlayers, 0.1f, true);
}

void AVbGameState::StopExtraction()
{
    if (!HasAuthority()) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(ExtractionTimer);
    ExtractionEndTimeServer = -1.f;
    OnExtractionCountdownUpdated.Broadcast(false);
}

void AVbGameState::ExtractPlayers() const
{
    if (!HasAuthority()) return;

    UWorld* World = GetWorld();
    if (!World) return;

    if (World->GetTimeSeconds() < ExtractionEndTimeServer) return;

    UVbGameInstance* GameInstance = GetGameInstance<UVbGameInstance>();
    if (!GameInstance) return;

    GameInstance->LeaveSession();
}

void AVbGameState::OnRep_ExtractionStartTimeServer()
{
    OnExtractionCountdownUpdated.Broadcast(ExtractionEndTimeServer > 0);
}