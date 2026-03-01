// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "VbItemList.generated.h"

class UVbItemInstance;
class UVbItemStatic;

USTRUCT()
struct FVbItemEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

public:
    FORCEINLINE UVbItemInstance* GetItemInstance() const { return ItemInstance; }
    FORCEINLINE void SetItemInstance(UVbItemInstance* InItemInstance) { ItemInstance = InItemInstance; }

private:
    UPROPERTY()
    UVbItemInstance* ItemInstance = nullptr;
};

USTRUCT()
struct FVbItemList : public FFastArraySerializer
{
    GENERATED_BODY()

public:
    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FastArrayDeltaSerialize<FVbItemEntry, FVbItemList>(Slots, DeltaParms, *this);
    }

    FORCEINLINE const TArray<FVbItemEntry>& GetSlots() const { return Slots; }
    FORCEINLINE TArray<FVbItemEntry>& GetSlots() { return Slots; }

    UVbItemInstance* GetItem(int32 Index) const;
    void SetItem(int32 Index, UVbItemInstance* Item);
    void SwapItems(int32 FromIndex, int32 ToIndex);
    void IncreaseQuantity(int32 Index, int32 Amount = 1);
    void DecreaseQuantity(int32 Index, int32 Amount = 1);
    void ClearItem(int32 Index);
    bool IsValidIndex(int32 Index) const;
    bool IsEmpty(int32 Index) const;

private:
    UPROPERTY()
    TArray<FVbItemEntry> Slots;
};

template <>
struct TStructOpsTypeTraits<FVbItemList> : public TStructOpsTypeTraitsBase2<FVbItemList>
{
    enum { WithNetDeltaSerializer = true };
};