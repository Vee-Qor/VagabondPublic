// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystem/VbItemList.h"
#include "VbInventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVbInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVbInventoryComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    FOnInventoryChanged OnInventoryChanged;

    void Initialize(const TMap<TSubclassOf<UVbItemStatic>, int32>& DefaultItemsMap, const int32 InMaxSlots = 36);

    void AddItem(UVbItemInstance* InItemInstance);
    void RemoveItem(int32 ItemIndex);
    void UseItem(int32 ItemIndex);
    void DropItem(int32 ItemIndex, int32 InQuantity = 1);
    void SwapSlots(int32 FromIndex, int32 ToIndex);

    UFUNCTION(Server, Reliable)
    void Server_DropItem(int32 ItemIndex, int32 InQuantity = 1);

    UFUNCTION(Server, Reliable)
    void Server_UseItem(int32 ItemIndex);

    UFUNCTION(Server, Reliable)
    void Server_SwapSlots(int32 FromIndex, int32 ToIndex);

    FORCEINLINE const TArray<FVbItemEntry>& GetItems() const { return InventoryList.GetSlots(); }
    FORCEINLINE TArray<FVbItemEntry>& GetItems() { return InventoryList.GetSlots(); }
    FORCEINLINE int32 GetMaxSlots() { return InventoryList.GetSlots().Num(); }
    UVbItemInstance* GetItem(const int32 Index) const;
    void SetItem(const int32 Index, UVbItemInstance* Instance);

    int32 FindFirstEmptySlot() const;

private:
    UFUNCTION()
    void OnRep_Inventory();

    void NotifyInventoryChanged() const;

    void SpawnDroppedItem(UVbItemInstance* InItemInstance) const;

    // ==== Utils ==== //
    bool HasAuthority() const;

    // ==== Runtime ==== //
    UPROPERTY(ReplicatedUsing=OnRep_Inventory)
    FVbItemList InventoryList;
};