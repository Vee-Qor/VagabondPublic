// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "ItemSystem/VbInventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Engine/ActorChannel.h"
#include "ItemSystem/VbItemActor.h"
#include "ItemSystem/VbItemInstance.h"
#include "Net/UnrealNetwork.h"

UVbInventoryComponent::UVbInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UVbInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UVbInventoryComponent, InventoryList);
}

bool UVbInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (const FVbItemEntry& Item : InventoryList.GetSlots())
    {
        if (UVbItemInstance* ItemInstance = Item.GetItemInstance())
        {
            bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }

    return bWroteSomething;
}

void UVbInventoryComponent::Initialize(const TMap<TSubclassOf<UVbItemStatic>, int32>& DefaultItemsMap, const int32 InMaxSlots)
{
    if (!HasAuthority()) return;

    InventoryList.GetSlots().SetNum(InMaxSlots);
    InventoryList.MarkArrayDirty();

    for (const auto& ItemMap : DefaultItemsMap)
    {
        if (const UVbItemStatic* ItemStatic = ItemMap.Key.GetDefaultObject())
        {
            UVbItemInstance* ItemInstance = NewObject<UVbItemInstance>(this);
            ItemInstance->Init(ItemStatic, ItemMap.Value);
            AddItem(ItemInstance);
        }
    }

    NotifyInventoryChanged();
}

void UVbInventoryComponent::AddItem(UVbItemInstance* InItemInstance)
{
    if (!HasAuthority() || !InItemInstance) return;

    TArray<FVbItemEntry>& Slots = InventoryList.GetSlots();

    for (FVbItemEntry& Slot : Slots)
    {
        UVbItemInstance* ItemInstance = Slot.GetItemInstance();
        if (!ItemInstance || !ItemInstance->ItemStatic) continue;

        const UVbItemStatic* ItemStatic = ItemInstance->ItemStatic;
        if (InItemInstance->ItemStatic != ItemStatic) continue;
        if (ItemInstance->Quantity == ItemStatic->MaxStackCount) continue;

        const int32 FreeSpace = ItemStatic->MaxStackCount - ItemInstance->Quantity;
        const int32 Added = FMath::Clamp(InItemInstance->Quantity, 0, FreeSpace);

        ItemInstance->Quantity += Added;
        InItemInstance->Quantity -= Added;

        InventoryList.MarkItemDirty(Slot);

        if (InItemInstance->Quantity == 0)
        {
            NotifyInventoryChanged();
            return;
        }
    }

    while (InItemInstance->Quantity > 0)
    {
        const int32 EmptyIndex = FindFirstEmptySlot();
        if (EmptyIndex == INDEX_NONE) return;

        const int32 StackQuantity = FMath::Min(InItemInstance->Quantity, InItemInstance->ItemStatic->MaxStackCount);

        UVbItemInstance* NewStack = DuplicateObject<UVbItemInstance>(InItemInstance, this);
        NewStack->Quantity = StackQuantity;

        SetItem(EmptyIndex, NewStack);

        InItemInstance->Quantity -= StackQuantity;
    }

    NotifyInventoryChanged();
}

void UVbInventoryComponent::RemoveItem(int32 ItemIndex)
{
    if (!HasAuthority()) return;

    InventoryList.ClearItem(ItemIndex);
    NotifyInventoryChanged();
}

void UVbInventoryComponent::DropItem(int32 ItemIndex, int32 InQuantity)
{
    if (InQuantity <= 0) return;

    if (!HasAuthority())
    {
        Server_DropItem(ItemIndex, InQuantity);
        return;
    }

    TArray<FVbItemEntry>& InventoryItems = InventoryList.GetSlots();
    if (!InventoryItems.IsValidIndex(ItemIndex)) return;

    FVbItemEntry& Item = InventoryItems[ItemIndex];

    UVbItemInstance* ItemInstance = Item.GetItemInstance();
    if (!ItemInstance || !ItemInstance->ItemStatic || ItemInstance->Quantity <= 0) return;

    const int32 DropQuantity = FMath::Min(InQuantity, ItemInstance->Quantity);

    UVbItemInstance* ItemInstanceToDrop = NewObject<UVbItemInstance>();
    ItemInstanceToDrop->Init(ItemInstance->ItemStatic, DropQuantity);

    ItemInstance->Quantity -= DropQuantity;
    InventoryList.MarkItemDirty(Item);

    if (ItemInstance->Quantity <= 0)
    {
        RemoveItem(ItemIndex);
    }

    SpawnDroppedItem(ItemInstanceToDrop);
    NotifyInventoryChanged();
}

void UVbInventoryComponent::UseItem(int32 ItemIndex)
{
    if (!HasAuthority())
    {
        Server_UseItem(ItemIndex);
        return;
    }

    TArray<FVbItemEntry>& InventoryItems = InventoryList.GetSlots();
    if (!InventoryItems.IsValidIndex(ItemIndex)) return;

    UVbAbilitySystemComponent* OwnerASC = Cast<UVbAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
    if (!OwnerASC) return;

    FVbItemEntry& Item = InventoryItems[ItemIndex];

    UVbItemInstance* ItemInstance = Item.GetItemInstance();
    if (!ItemInstance || !ItemInstance->ItemStatic || ItemInstance->Quantity <= 0) return;

    if (ItemInstance->ItemStatic->Effects.IsEmpty()) return;

    OwnerASC->ApplyEffects(ItemInstance->ItemStatic->Effects);

    InventoryList.DecreaseQuantity(ItemIndex);
    NotifyInventoryChanged();
}

void UVbInventoryComponent::SwapSlots(int32 FromIndex, int32 ToIndex)
{
    if (FromIndex == ToIndex) return;

    if (!HasAuthority())
    {
        Server_SwapSlots(FromIndex, ToIndex);
        return;
    }

    InventoryList.SwapItems(FromIndex, ToIndex);
    NotifyInventoryChanged();
}

void UVbInventoryComponent::Server_DropItem_Implementation(int32 ItemIndex, int32 InQuantity)
{
    DropItem(ItemIndex, InQuantity);
}

void UVbInventoryComponent::Server_UseItem_Implementation(int32 ItemIndex)
{
    UseItem(ItemIndex);
}

void UVbInventoryComponent::Server_SwapSlots_Implementation(int32 FromIndex, int32 ToIndex)
{
    SwapSlots(FromIndex, ToIndex);
}

bool UVbInventoryComponent::HasAuthority() const
{
    return GetOwnerRole() == ROLE_Authority;
}

void UVbInventoryComponent::SpawnDroppedItem(UVbItemInstance* InItemInstance) const
{
    if (!InItemInstance) return;

    const AActor* Owner = GetOwner();
    if (!Owner) return;

    const FVector OwnerLocation = Owner->GetActorLocation();
    const FVector OwnerForward = Owner->GetActorForwardVector();

    const FVector SpawnLocation = OwnerLocation + OwnerForward * 100.0f;
    const FRotator SpawnRotation = Owner->GetActorRotation();

    const FTransform DropTransform(SpawnRotation, SpawnLocation);

    const UVbItemStatic* ItemStatic = InItemInstance->ItemStatic;
    if (!ItemStatic || !ItemStatic->ItemActorClass) return;

    UWorld* World = GetWorld();
    if (!World) return;

    AVbItemActor* SpawnedItem = World->SpawnActorDeferred<AVbItemActor>(ItemStatic->ItemActorClass, DropTransform);
    SpawnedItem->Init(InItemInstance);
    SpawnedItem->FinishSpawning(DropTransform);
}

int32 UVbInventoryComponent::FindFirstEmptySlot() const
{
    const TArray<FVbItemEntry>& Slots = InventoryList.GetSlots();

    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (Slots[i].GetItemInstance()) continue;

        return i;
    }

    return INDEX_NONE;
}

void UVbInventoryComponent::OnRep_Inventory()
{
    NotifyInventoryChanged();
}

void UVbInventoryComponent::NotifyInventoryChanged() const
{
    OnInventoryChanged.Broadcast();
}

UVbItemInstance* UVbInventoryComponent::GetItem(const int32 Index) const
{
    return InventoryList.GetItem(Index);
}

void UVbInventoryComponent::SetItem(const int32 Index, UVbItemInstance* Instance)
{
    if (!HasAuthority()) return;

    InventoryList.SetItem(Index, Instance);
    NotifyInventoryChanged();
}