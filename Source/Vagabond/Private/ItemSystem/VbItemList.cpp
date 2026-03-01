// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "ItemSystem/VbItemList.h"

#include "VbItemInstance.h"

UVbItemInstance* FVbItemList::GetItem(int32 Index) const
{
    if (!IsValidIndex(Index)) return nullptr;

    return Slots[Index].GetItemInstance();
}

void FVbItemList::SetItem(int32 Index, UVbItemInstance* Item)
{
    if (!IsValidIndex(Index)) return;

    Slots[Index].SetItemInstance(Item);
    MarkItemDirty(Slots[Index]);
}

void FVbItemList::SwapItems(int32 FromIndex, int32 ToIndex)
{
    if (FromIndex == ToIndex || !IsValidIndex(FromIndex) || !IsValidIndex(ToIndex)) return;

    UVbItemInstance* Temp = Slots[FromIndex].GetItemInstance();
    Slots[FromIndex].SetItemInstance(Slots[ToIndex].GetItemInstance());
    Slots[ToIndex].SetItemInstance(Temp);

    MarkItemDirty(Slots[FromIndex]);
    MarkItemDirty(Slots[ToIndex]);
}

void FVbItemList::IncreaseQuantity(int32 Index, int32 Amount)
{
    if (!IsValidIndex(Index) || Amount <= 0) return;

    FVbItemEntry& Slot = Slots[Index];

    UVbItemInstance* Item = Slot.GetItemInstance();
    if (!Item || !Item->ItemStatic) return;

    Item->Quantity = FMath::Clamp(Item->Quantity + Amount, 1, Item->ItemStatic->MaxStackCount);

    MarkItemDirty(Slot);
}

void FVbItemList::DecreaseQuantity(int32 Index, int32 Amount)
{
    if (!IsValidIndex(Index) || Amount <= 0) return;

    FVbItemEntry& Slot = Slots[Index];

    UVbItemInstance* Item = Slot.GetItemInstance();
    if (!Item || !Item->ItemStatic) return;

    Item->Quantity = FMath::Max(0, Item->Quantity - Amount);

    if (Item->Quantity == 0)
    {
        Slot.SetItemInstance(nullptr);
    }

    MarkItemDirty(Slot);
}

bool FVbItemList::IsValidIndex(int32 Index) const
{
    return Slots.IsValidIndex(Index);
}

bool FVbItemList::IsEmpty(int32 Index) const
{
    if (!IsValidIndex(Index)) return false;

    return Slots[Index].GetItemInstance() == nullptr;
}

void FVbItemList::ClearItem(int32 Index)
{
    if (!IsValidIndex(Index)) return;

    Slots[Index].SetItemInstance(nullptr);
    MarkItemDirty(Slots[Index]);
}