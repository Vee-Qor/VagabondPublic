// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Inventory/VbInventoryWidget.h"

#include "VbCoreTypes.h"
#include "Components/TileView.h"
#include "VbInventorySlotWidget.h"
#include "ItemSystem/VbInventoryComponent.h"

void UVbInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent) return;

    InventoryComponent->OnInventoryChanged.AddUObject(this, &UVbInventoryWidget::OnInventoryChanged);
    OnInventoryChanged();
}

void UVbInventoryWidget::ToggleInventory()
{
    if (GetVisibility() == ESlateVisibility::Collapsed)
    {
        SetVisibility(ESlateVisibility::Visible);
        SetInputEnabled(false);
        SetShowMouseCursor(true);
        SetFocusToGameAndUI();
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
        SetInputEnabled(true);
        SetShowMouseCursor(false);
        SetFocusToGameOnly();
    }
}

void UVbInventoryWidget::OnInventoryChanged()
{
    if (!InventoryList || !InventoryComponent) return;

    InventoryList->ClearListItems();

    const TArray<FVbItemEntry>& Slots = InventoryComponent->GetItems();
    const int32 MaxSlots = InventoryComponent->GetMaxSlots();

    for (int32 Index = 0; Index < MaxSlots; ++Index)
    {
        UVbSlotObject* SlotObject = NewObject<UVbSlotObject>(this);
        SlotObject->SlotIndex = Index;

        if (Slots.IsValidIndex(Index))
        {
            SlotObject->ItemInstance = Slots[Index].GetItemInstance();
        }

        InventoryList->AddItem(SlotObject);
    }
}