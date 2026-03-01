// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Inventory/VbEquipmentSlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "VbInventorySlotWidget.h"
#include "VbItemToolTipWidget.h"
#include "ItemSystem/VbEquipmentComponent.h"
#include "ItemSystem/VbItemInstance.h"

void UVbEquipmentSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EquipmentComponent = GetEquipmentComponent();
}

void UVbEquipmentSlotWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (SlotIcon)
    {
        SlotIcon->SetBrushFromTexture(SlotImage);
    }
}

FReply UVbEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if (InMouseEvent.IsShiftDown())
        {
            RequestUnequipItem();
            return FReply::Handled();
        }
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return EventReply.NativeReply;
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UVbEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!ItemInstance || !ItemInstance->ItemStatic || ItemInstance->Quantity <= 0)
    {
        return;
    }

    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    if (UDragDropOperation* DragDropOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()))
    {
        const FVector2D SlotSize = InGeometry.GetLocalSize();
        UImage* DragImage = NewObject<UImage>(this);
        FSlateBrush Brush;
        Brush.SetResourceObject(ItemInstance->ItemStatic->Icon);
        Brush.ImageSize = SlotSize;
        DragImage->SetBrush(Brush);

        DragDropOperation->Payload = this;
        DragDropOperation->DefaultDragVisual = DragImage;
        DragDropOperation->Pivot = EDragPivot::CenterCenter;

        OutOperation = DragDropOperation;
    }
}

bool UVbEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (!InOperation) return false;

    if (const UVbInventorySlotWidget* FromSlot = Cast<UVbInventorySlotWidget>(InOperation->Payload))
    {
        if (!EquipmentComponent) return false;
        const int32 EquipmentIndex = EquipmentComponent->FindEquipmentIndexBySlot(SlotTag);

        RequestEquipItem(FromSlot->GetSlotIndex(), EquipmentIndex);
        return true;
    }

    if (const UVbEquipmentSlotWidget* FromSlot = Cast<UVbEquipmentSlotWidget>(InOperation->Payload))
    {
        if (!EquipmentComponent) return false;

        const int32 FromIndex = EquipmentComponent->FindEquipmentIndexBySlot(FromSlot->GetSlotTag());
        const int32 ToIndex = EquipmentComponent->FindEquipmentIndexBySlot(GetSlotTag());

        RequestSwapEquipment(FromIndex, ToIndex);
        return true;
    }

    return false;
}

void UVbEquipmentSlotWidget::SetItem(UVbItemInstance* Item)
{
    if (!ItemIcon) return;

    if (Item && Item->ItemStatic && Item->ItemStatic->Icon)
    {
        ItemInstance = Item;
        ItemIcon->SetBrushFromTexture(Item->ItemStatic->Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemInstance = nullptr;
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    CreateToolTipWidget();
}

void UVbEquipmentSlotWidget::CreateToolTipWidget()
{
    if (!ItemToolTipWidgetClass || !ItemInstance || !ItemInstance->ItemStatic) return;

    UVbItemToolTipWidget* ItemToolTip = CreateWidget<UVbItemToolTipWidget>(this, ItemToolTipWidgetClass);
    if (!ItemToolTip) return;

    ItemToolTip->SetInfo(ItemInstance->ItemStatic->Icon, ItemInstance->ItemStatic->Name, ItemInstance->ItemStatic->Description);
    SetToolTip(ItemToolTip);
}

void UVbEquipmentSlotWidget::RequestEquipItem(int32 InventoryIndex, int32 EquipmentIndex)
{
    if (EquipmentComponent)
    {
        EquipmentComponent->Equip(InventoryIndex, EquipmentIndex);
    }
}

void UVbEquipmentSlotWidget::RequestSwapEquipment(int32 FromIndex, int32 ToIndex)
{
    if (EquipmentComponent)
    {
        EquipmentComponent->SwapSlots(FromIndex, ToIndex);
    }
}

void UVbEquipmentSlotWidget::RequestUnequipItem()
{
    if (EquipmentComponent)
    {
        EquipmentComponent->UnequipAuto(EquipmentComponent->FindEquipmentIndexBySlot(SlotTag));
    }
}