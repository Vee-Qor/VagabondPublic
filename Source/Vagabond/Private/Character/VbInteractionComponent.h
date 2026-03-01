// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VbInteractionComponent.generated.h"

class AVbItemActor;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChangedDelegate, bool /*bHave*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVbInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVbInteractionComponent();

    FOnInteractionTargetChangedDelegate OnInteractionTargetChanged;

    void TryPickupItem();
    void AddPickupTarget(AVbItemActor* InPickupTarget);
    void RemovePickupTarget(AVbItemActor* InPickupTarget);

private:
    UFUNCTION(Server, Reliable)
    void Server_PickupItem(AVbItemActor* ItemToPickup);

    void NotifyInteractionTargetChanged();

    UPROPERTY()
    TSet<AVbItemActor*> PickupTargets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    float MaxDistanceInteraction = 1000.0f;
};