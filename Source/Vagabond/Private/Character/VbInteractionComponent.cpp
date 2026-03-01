// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbInteractionComponent.h"

#include "Components/SphereComponent.h"
#include "ItemSystem/VbItemActor.h"
#include "ItemSystem/VbItemInstance.h"
#include "ItemSystem/VbInventoryComponent.h"

UVbInteractionComponent::UVbInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UVbInteractionComponent::AddPickupTarget(AVbItemActor* InPickupTarget)
{
    if (!InPickupTarget) return;

    PickupTargets.Add(InPickupTarget);
    NotifyInteractionTargetChanged();
}

void UVbInteractionComponent::RemovePickupTarget(AVbItemActor* InPickupTarget)
{
    if (!InPickupTarget) return;

    PickupTargets.Remove(InPickupTarget);
    NotifyInteractionTargetChanged();
}

void UVbInteractionComponent::NotifyInteractionTargetChanged()
{
    if (PickupTargets.IsEmpty())
    {
        OnInteractionTargetChanged.Broadcast(false);
        return;
    }

    for (const AActor* Target : PickupTargets)
    {
        if (!Target) continue;

        OnInteractionTargetChanged.Broadcast(true);
        return;
    }

    OnInteractionTargetChanged.Broadcast(false);
}

void UVbInteractionComponent::TryPickupItem()
{
    if (PickupTargets.IsEmpty()) return;

    const FVector ActorLocation = GetOwner()->GetActorLocation();
    float BestItemDistance = MaxDistanceInteraction * MaxDistanceInteraction;
    AVbItemActor* ItemToPickup = nullptr;

    for (AVbItemActor* ItemTarget : PickupTargets)
    {
        if (!ItemTarget) continue;

        FVector ItemLocation = ItemTarget->GetActorLocation();

        const float ItemDistance = FVector::DistSquared(ActorLocation, ItemLocation);

        if (BestItemDistance <= ItemDistance) continue;

        BestItemDistance = ItemDistance;
        ItemToPickup = ItemTarget;
    }

    if (!ItemToPickup) return;
    Server_PickupItem(ItemToPickup);
}

void UVbInteractionComponent::Server_PickupItem_Implementation(AVbItemActor* ItemToPickup)
{
    if (!ItemToPickup) return;

    const USphereComponent* PickupCollision = ItemToPickup->GetPickupCollision();
    if (!PickupCollision || !PickupCollision->IsOverlappingActor(GetOwner())) return;

    const AActor* Owner = GetOwner();
    if (!Owner) return;

    UVbInventoryComponent* InventoryComponent = Owner->FindComponentByClass<UVbInventoryComponent>();
    if (!InventoryComponent) return;

    UVbItemInstance* ItemInstance = ItemToPickup->GetItemInstance();
    if (!ItemInstance) return;

    InventoryComponent->AddItem(ItemInstance);
    ItemToPickup->Destroy();
}