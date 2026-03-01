// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "VbGameState.generated.h"

class AVbExtractionZone;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExtractionCountdownUpdatedDelegate, bool /*bIsActive*/);

UCLASS()
class AVbGameState : public AGameState
{
    GENERATED_BODY()

public:
    FOnExtractionCountdownUpdatedDelegate OnExtractionCountdownUpdated;

    void StartExtraction();
    void StopExtraction();

    FORCEINLINE float GetExtractionEndTimeServer() const { return ExtractionEndTimeServer; }

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void ExtractPlayers() const;

    UFUNCTION()
    void OnRep_ExtractionStartTimeServer();

    UPROPERTY(EditAnywhere, Category="Time")
    float ExtractionTimeDelay = 10.0f;

    FTimerHandle ExtractionTimer;

    UPROPERTY(ReplicatedUsing=OnRep_ExtractionStartTimeServer)
    float ExtractionEndTimeServer = -1.0f;
};