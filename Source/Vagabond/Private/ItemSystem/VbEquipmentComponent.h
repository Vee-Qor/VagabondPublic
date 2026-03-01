// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ItemSystem/VbItemList.h"
#include "VbEquipmentComponent.generated.h"

class UVbCombatComponent;
class UVbInventoryComponent;

DECLARE_MULTICAST_DELEGATE(FOnEquipmentChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UVbEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVbEquipmentComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    FOnEquipmentChanged OnEquipmentChanged;

    void Equip(int32 InventoryIndex, int32 EquipmentIndex);
    void EquipAuto(int32 InventoryIndex);
    void Unequip(int32 InventoryIndex, int32 EquipmentIndex);
    void UnequipAuto(int32 EquipmentIndex);
    void SwapSlots(int32 IndexOne, int32 IndexTwo);

    void ApplyEquipItem(UVbItemInstance* ItemInstance, int32 SlotIndex);
    void RemoveEquipItem(UVbItemInstance* ItemInstance, int32 SlotIndex);

    UFUNCTION(Server, Reliable)
    void Server_Equip(int32 InventoryIndex, int32 EquipmentIndex);
    UFUNCTION(Server, Reliable)
    void Server_EquipAuto(int32 InventoryIndex);
    UFUNCTION(Server, Reliable)
    void Server_Unequip(int32 InventoryIndex, int32 EquipmentIndex);
    UFUNCTION(Server, Reliable)
    void Server_UnequipAuto(int32 EquipmentIndex);
    UFUNCTION(Server, Reliable)
    void Server_SwapSlots(int32 IndexOne, int32 IndexTwo);

    FORCEINLINE const TArray<FVbItemEntry>& GetItems() const { return EquipmentList.GetSlots(); }
    FORCEINLINE TArray<FVbItemEntry>& GetItems() { return EquipmentList.GetSlots(); }
    int32 FindEquipmentIndexBySlot(const FGameplayTag& SlotTag) const;
    const FGameplayTag& GetSlotTagByIndex(const int32 Index) const;
    FORCEINLINE UVbItemInstance* GetItem(int32 Index) const { return EquipmentList.GetItem(Index); }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(ReplicatedUsing=OnRep_EquipmentList)
    FVbItemList EquipmentList;

    UFUNCTION()
    void OnRep_EquipmentList() const;

    void NotifyEquipmentChanged() const;

    // ==== Utils ==== //
    bool HasAuthority() const;

    UPROPERTY(EditDefaultsOnly, Category="Equipment")
    TArray<FGameplayTag> SlotTags;

    static TArray<FGameplayTag> MakeDefaultSlotTags();

    UVbInventoryComponent* GetInventoryComponent() const;
    UVbCombatComponent* GetCombatComponent() const;

    bool CanEquipToIndex(const UVbItemInstance* Item, int32 EquipmentIndex) const;
};