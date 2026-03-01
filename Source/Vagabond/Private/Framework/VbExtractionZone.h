// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VbExtractionZone.generated.h"

class UBoxComponent;

UCLASS()
class AVbExtractionZone : public AActor
{
    GENERATED_BODY()

public:
    AVbExtractionZone();

protected:
    virtual void BeginPlay() override;

private:
    bool IsPlayer(AActor* Actor) const;
    bool IsExtractionReady() const;

    UFUNCTION()
    void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxOverlap;

    // ==== Runtime ==== //
    UPROPERTY()
    TSet<APlayerState*> PlayersInZone;

};