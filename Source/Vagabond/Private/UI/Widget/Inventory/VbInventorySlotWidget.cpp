// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Inventory/VbInventorySlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "VbCoreTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "VbEquipmentSlotWidget.h"
#include "VbItemToolTipWidget.h"
#include "ItemSystem/VbEquipmentComponent.h"
#include "ItemSystem/VbItemInstance.h"
#include "ItemSystem/VbInventoryComponent.h"

void UVbInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InventoryComponent = GetInventoryComponent();
    EquipmentComponent = GetEquipmentComponent();
}

void UVbInventorySlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    SlotObject = Cast<UVbSlotObject>(ListItemObject);
    if (SlotObject)
    {
        InitializeSlot(SlotObject->ItemInstance);
    }
}

FReply UVbInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!ItemInstance || !ItemInstance->ItemStatic || ItemInstance->Quantity <= 0)
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if (InMouseEvent.IsControlDown())
        {
            RequestDropItem();
            return FReply::Handled();
        }

        if (InMouseEvent.IsShiftDown())
        {
            RequestEquipItem();
            return FReply::Handled();
        }

        RequestUseItem();
        return FReply::Handled();
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FEventReply EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return EventReply.NativeReply;
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UVbInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!ItemInstance || !ItemInstance->ItemStatic || ItemInstance->Quantity <= 0)
    {
        return;
    }

    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    UDragDropOperation* DragDropOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    if (!DragDropOperation) return;

    DragDropOperation->Payload = this;
    DragDropOperation->Pivot = EDragPivot::CenterCenter;

    const FVector2D SlotSize = InGeometry.GetLocalSize();

    UImage* DragImage = NewObject<UImage>(this);
    FSlateBrush Brush;
    Brush.SetResourceObject(ItemInstance->ItemStatic->Icon);
    Brush.ImageSize = SlotSize;
    DragImage->SetBrush(Brush);

    DragDropOperation->DefaultDragVisual = DragImage;

    OutOperation = DragDropOperation;
}

bool UVbInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (!InOperation) return false;

    if (const UVbInventorySlotWidget* FromSlot = Cast<UVbInventorySlotWidget>(InOperation->Payload))
    {
        RequestSwapItems(FromSlot->GetSlotIndex(), GetSlotIndex());
        return true;
    }

    if (const UVbEquipmentSlotWidget* FromSlot = Cast<UVbEquipmentSlotWidget>(InOperation->Payload))
    {
        if (!EquipmentComponent) return false;

        const int32 EquipmentIndex = EquipmentComponent->FindEquipmentIndexBySlot(FromSlot->GetSlotTag());

        RequestUnequip(EquipmentIndex);
        return true;
    }

    return false;
}

void UVbInventorySlotWidget::InitializeSlot(UVbItemInstance* InItemInstance)
{
    if (!ItemIcon || !CountText) return;

    ItemInstance = InItemInstance;

    if (ItemInstance && ItemInstance->ItemStatic)
    {
        ItemIcon->SetBrushFromTexture(ItemInstance->ItemStatic->Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);

        CountText->SetText(FText::AsNumber(ItemInstance->Quantity));
        CountText->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        CountText->SetVisibility(ESlateVisibility::Hidden);
    }

    CreateToolTipWidget();
}

void UVbInventorySlotWidget::CreateToolTipWidget()
{
    if (!ItemToolTipWidgetClass || !ItemInstance || !ItemInstance->ItemStatic) return;

    UVbItemToolTipWidget* ItemToolTip = CreateWidget<UVbItemToolTipWidget>(this, ItemToolTipWidgetClass);
    if (!ItemToolTip) return;

    ItemToolTip->SetInfo(ItemInstance->ItemStatic->Icon, ItemInstance->ItemStatic->Name, ItemInstance->ItemStatic->Description);
    SetToolTip(ItemToolTip);
}

void UVbInventorySlotWidget::RequestUseItem() const
{
    if (InventoryComponent)
    {
        InventoryComponent->UseItem(GetSlotIndex());
    }
}

void UVbInventorySlotWidget::RequestDropItem() const
{
    if (InventoryComponent)
    {
        InventoryComponent->DropItem(GetSlotIndex());
    }
}

void UVbInventorySlotWidget::RequestSwapItems(const int32 FromIndex, const int32 ToIndex) const
{
    if (InventoryComponent)
    {
        InventoryComponent->SwapSlots(FromIndex, ToIndex);
    }
}

void UVbInventorySlotWidget::RequestEquipItem() const
{
    if (EquipmentComponent)
    {
        EquipmentComponent->EquipAuto(GetSlotIndex());
    }
}

void UVbInventorySlotWidget::RequestUnequip(int32 EquipmentIndex) const
{
    if (EquipmentComponent)
    {
        EquipmentComponent->Unequip(GetSlotIndex(), EquipmentIndex);
    }
}

int32 UVbInventorySlotWidget::GetSlotIndex() const
{
    if (SlotObject)
    {
        return SlotObject->SlotIndex;
    }

    return INDEX_NONE;
}