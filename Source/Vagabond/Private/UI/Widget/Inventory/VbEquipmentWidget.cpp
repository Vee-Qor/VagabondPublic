// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Inventory/VbEquipmentWidget.h"

#include "Components/UniformGridPanel.h"
#include "VbEquipmentSlotWidget.h"
#include "ItemSystem/VbEquipmentComponent.h"
#include "ItemSystem/VbItemInstance.h"

void UVbEquipmentWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EquipmentComponent = GetEquipmentComponent();
    if (!EquipmentComponent) return;

    EquipmentComponent->OnEquipmentChanged.AddUObject(this, &UVbEquipmentWidget::OnEquipmentChanged);
    OnEquipmentChanged();
}

void UVbEquipmentWidget::ToggleEquipment()
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

void UVbEquipmentWidget::OnEquipmentChanged() const
{
    if (!EquipmentComponent || !EquipmentPanel) return;

    const TArray<FVbItemEntry>& EquipmentSlots = EquipmentComponent->GetItems();

    for (int32 i = 0; i < EquipmentPanel->GetChildrenCount(); i++)
    {
        UVbEquipmentSlotWidget* SlotWidget = Cast<UVbEquipmentSlotWidget>(EquipmentPanel->GetChildAt(i));
        if (!SlotWidget) continue;

        const int32 EquipmentIndex = EquipmentComponent->FindEquipmentIndexBySlot(SlotWidget->GetSlotTag());

        UVbItemInstance* ItemInstance = nullptr;

        if (EquipmentSlots.IsValidIndex(EquipmentIndex))
        {
            ItemInstance = EquipmentSlots[EquipmentIndex].GetItemInstance();
        }

        SlotWidget->SetItem(ItemInstance);
    }
}