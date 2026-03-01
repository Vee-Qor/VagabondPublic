// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "VbMenuGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSessionPlayersChangedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionStartTimerDelegate, bool /*bActive*/);

UCLASS()
class AVbMenuGameState : public AGameState
{
    GENERATED_BODY()

public:
    FOnSessionPlayersChangedDelegate OnSessionPlayersChanged;
    FOnSessionStartTimerDelegate OnSessionStartTimer;

    FORCEINLINE float GetMatchStartDelay() const { return MatchStartDelay; }
    FORCEINLINE float GetMatchStartTimeServer() const { return MatchStartTimeServer; }

protected:
    virtual void AddPlayerState(APlayerState* PlayerState) override;
    virtual void RemovePlayerState(APlayerState* PlayerState) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void TryStartMatch();
    void ResetMatchStart();
    void StartMatchCountdown();
    void CancelMatchCountdown();
    bool IsAllReady() const;

    UFUNCTION()
    void OnReadyChanged(bool bReady);

    UFUNCTION()
    void OnRep_MatchStartTimeServer();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly)
    float MatchStartDelay = 5.0f;

    // ==== Runtime ==== //
    FTimerHandle MatchStartTimerHandle;

    UPROPERTY(ReplicatedUsing=OnRep_MatchStartTimeServer)
    float MatchStartTimeServer = 0.0f;
};