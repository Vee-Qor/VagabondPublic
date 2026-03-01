// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "ItemSystem/VbEquipmentComponent.h"

#include "Character/VbCombatComponent.h"
#include "Engine/ActorChannel.h"
#include "VbGameplayTags.h"
#include "VbInventoryComponent.h"
#include "VbItemInstance.h"
#include "Net/UnrealNetwork.h"

UVbEquipmentComponent::UVbEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);

    SlotTags = MakeDefaultSlotTags();
}

void UVbEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UVbEquipmentComponent, EquipmentList);
}

bool UVbEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (const FVbItemEntry& Item : EquipmentList.GetSlots())
    {
        if (UVbItemInstance* ItemInstance = Item.GetItemInstance())
        {
            bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }

    return bWroteSomething;
}

void UVbEquipmentComponent::Equip(int32 InventoryIndex, int32 EquipmentIndex)
{
    if (InventoryIndex == INDEX_NONE || EquipmentIndex == INDEX_NONE) return;

    if (!HasAuthority())
    {
        Server_Equip(InventoryIndex, EquipmentIndex);
        return;
    }

    UVbInventoryComponent* InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent) return;

    UVbItemInstance* EquipItem = EquipmentList.GetItem(EquipmentIndex);
    UVbItemInstance* InventoryItem = InventoryComponent->GetItem(InventoryIndex);

    if (!CanEquipToIndex(InventoryItem, EquipmentIndex)) return;

    RemoveEquipItem(EquipItem, EquipmentIndex);

    InventoryComponent->SetItem(InventoryIndex, EquipItem);
    EquipmentList.SetItem(EquipmentIndex, InventoryItem);

    ApplyEquipItem(InventoryItem, EquipmentIndex);

    NotifyEquipmentChanged();
}

void UVbEquipmentComponent::EquipAuto(int32 InventoryIndex)
{
    if (InventoryIndex == INDEX_NONE) return;

    if (!HasAuthority())
    {
        Server_EquipAuto(InventoryIndex);
        return;
    }

    const UVbInventoryComponent* InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent) return;

    const UVbItemInstance* Item = InventoryComponent->GetItem(InventoryIndex);
    if (!Item || !Item->ItemStatic) return;

    int32 BestEquipmentIndex = INDEX_NONE;

    for (int32 i = 0; i < SlotTags.Num(); ++i)
    {
        if (!CanEquipToIndex(Item, i)) continue;

        if (!EquipmentList.GetItem(i))
        {
            BestEquipmentIndex = i;
            break;
        }

        if (BestEquipmentIndex == INDEX_NONE)
        {
            BestEquipmentIndex = i;
        }
    }

    if (BestEquipmentIndex == INDEX_NONE) return;

    Equip(InventoryIndex, BestEquipmentIndex);
}

void UVbEquipmentComponent::Unequip(int32 InventoryIndex, int32 EquipmentIndex)
{
    if (InventoryIndex == INDEX_NONE || EquipmentIndex == INDEX_NONE) return;

    if (!HasAuthority())
    {
        Server_Unequip(InventoryIndex, EquipmentIndex);
        return;
    }

    UVbInventoryComponent* InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent) return;

    UVbItemInstance* EquipItem = EquipmentList.GetItem(EquipmentIndex);
    UVbItemInstance* InventoryItem = InventoryComponent->GetItem(InventoryIndex);

    if (InventoryItem)
    {
        if (!CanEquipToIndex(InventoryItem, EquipmentIndex)) return;
    }

    RemoveEquipItem(EquipItem, EquipmentIndex);

    InventoryComponent->SetItem(InventoryIndex, EquipItem);
    EquipmentList.SetItem(EquipmentIndex, InventoryItem);

    ApplyEquipItem(InventoryItem, EquipmentIndex);

    NotifyEquipmentChanged();
}

void UVbEquipmentComponent::UnequipAuto(int32 EquipmentIndex)
{
    if (EquipmentIndex == INDEX_NONE) return;

    if (!HasAuthority())
    {
        Server_UnequipAuto(EquipmentIndex);
        return;
    }

    const UVbInventoryComponent* InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent) return;

    const int32 EmptySlotIndex = InventoryComponent->FindFirstEmptySlot();
    if (EmptySlotIndex == INDEX_NONE) return;

    Unequip(EmptySlotIndex, EquipmentIndex);
}

void UVbEquipmentComponent::Server_UnequipAuto_Implementation(int32 EquipmentIndex)
{
    UnequipAuto(EquipmentIndex);
}

void UVbEquipmentComponent::Server_EquipAuto_Implementation(int32 InventoryIndex)
{
    EquipAuto(InventoryIndex);
}

void UVbEquipmentComponent::Server_Equip_Implementation(int32 InventoryIndex, int32 EquipmentIndex)
{
    Equip(InventoryIndex, EquipmentIndex);
}

void UVbEquipmentComponent::Server_Unequip_Implementation(int32 InventoryIndex, int32 EquipmentIndex)
{
    Unequip(InventoryIndex, EquipmentIndex);
}

void UVbEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        EquipmentList.GetSlots().SetNum(SlotTags.Num());
        EquipmentList.MarkArrayDirty();
    }
}

void UVbEquipmentComponent::OnRep_EquipmentList() const
{
    NotifyEquipmentChanged();
}

void UVbEquipmentComponent::NotifyEquipmentChanged() const
{
    OnEquipmentChanged.Broadcast();
}

bool UVbEquipmentComponent::HasAuthority() const
{
    return GetOwnerRole() == ROLE_Authority;
}

TArray<FGameplayTag> UVbEquipmentComponent::MakeDefaultSlotTags()
{
    return {
        VbGameplayTags::Equipment_Slot_Helmet,
        VbGameplayTags::Equipment_Slot_Chest,
        VbGameplayTags::Equipment_Slot_Belt,
        VbGameplayTags::Equipment_Slot_Gloves,
        VbGameplayTags::Equipment_Slot_Boots,
        VbGameplayTags::Equipment_Slot_Necklace,
        VbGameplayTags::Equipment_Slot_RingOne,
        VbGameplayTags::Equipment_Slot_RingTwo,
        VbGameplayTags::Equipment_Slot_FirstWeapon,
        VbGameplayTags::Equipment_Slot_SecondWeapon,
    };
}

int32 UVbEquipmentComponent::FindEquipmentIndexBySlot(const FGameplayTag& SlotTag) const
{
    return SlotTags.IndexOfByKey(SlotTag);
}

const FGameplayTag& UVbEquipmentComponent::GetSlotTagByIndex(const int32 Index) const
{
    return SlotTags[Index];
}

UVbInventoryComponent* UVbEquipmentComponent::GetInventoryComponent() const
{
    const AActor* Pawn = GetOwner();

    return Pawn ? Pawn->FindComponentByClass<UVbInventoryComponent>() : nullptr;
}

UVbCombatComponent* UVbEquipmentComponent::GetCombatComponent() const
{
    const AActor* OwnerActor = GetOwner();

    return OwnerActor ? OwnerActor->FindComponentByClass<UVbCombatComponent>() : nullptr;
}

bool UVbEquipmentComponent::CanEquipToIndex(const UVbItemInstance* Item, int32 EquipmentIndex) const
{
    if (!Item || !Item->ItemStatic) return false;

    if (!SlotTags.IsValidIndex(EquipmentIndex)) return false;

    if (!Item->ItemStatic->IsEquippable()) return false;

    if (Item->Quantity > 1) return false;

    return Item->ItemStatic->AllowedEquipSlots.HasTagExact(SlotTags[EquipmentIndex]);
}

void UVbEquipmentComponent::SwapSlots(int32 IndexOne, int32 IndexTwo)
{
    if (IndexOne == INDEX_NONE || IndexTwo == INDEX_NONE || IndexOne == IndexTwo) return;

    if (!HasAuthority())
    {
        Server_SwapSlots(IndexOne, IndexTwo);
        return;
    }

    UVbItemInstance* ItemOne = GetItem(IndexOne);
    UVbItemInstance* ItemTwo = GetItem(IndexTwo);

    if (ItemOne && !CanEquipToIndex(ItemOne, IndexTwo)) return;
    if (ItemTwo && !CanEquipToIndex(ItemTwo, IndexOne)) return;

    RemoveEquipItem(ItemOne, IndexOne);
    RemoveEquipItem(ItemTwo, IndexTwo);

    EquipmentList.SwapItems(IndexOne, IndexTwo);

    ApplyEquipItem(GetItem(IndexOne), IndexOne);
    ApplyEquipItem(GetItem(IndexTwo), IndexTwo);

    NotifyEquipmentChanged();
}

void UVbEquipmentComponent::ApplyEquipItem(UVbItemInstance* ItemInstance, int32 SlotIndex)
{
    if (!HasAuthority()) return;

    if (!ItemInstance || !ItemInstance->ItemStatic) return;

    if (!SlotTags.IsValidIndex(SlotIndex)) return;

    const FGameplayTag& SlotTag = SlotTags[SlotIndex];
    const bool bIsWeaponSlot = SlotTag == VbGameplayTags::Equipment_Slot_FirstWeapon || SlotTag == VbGameplayTags::Equipment_Slot_SecondWeapon;
    if (!bIsWeaponSlot) return;

    UVbStyleData* StyleData = ItemInstance->ItemStatic->StyleData;
    if (!StyleData) return;

    UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return;

    CombatComponent->SetWeaponStyle(StyleData);
}

void UVbEquipmentComponent::RemoveEquipItem(UVbItemInstance* ItemInstance, int32 SlotIndex)
{
    if (!HasAuthority()) return;

    if (!ItemInstance || !ItemInstance->ItemStatic) return;

    if (!SlotTags.IsValidIndex(SlotIndex)) return;

    const FGameplayTag& SlotTag = SlotTags[SlotIndex];
    const bool bIsWeaponSlot = SlotTag == VbGameplayTags::Equipment_Slot_FirstWeapon || SlotTag == VbGameplayTags::Equipment_Slot_SecondWeapon;
    if (!bIsWeaponSlot) return;

    UVbStyleData* StyleData = ItemInstance->ItemStatic->StyleData;
    if (!StyleData) return;

    UVbCombatComponent* CombatComponent = GetCombatComponent();
    if (!CombatComponent) return;

    CombatComponent->RemoveWeaponStyle(StyleData);
}

void UVbEquipmentComponent::Server_SwapSlots_Implementation(int32 IndexOne, int32 IndexTwo)
{
    SwapSlots(IndexOne, IndexTwo);
}