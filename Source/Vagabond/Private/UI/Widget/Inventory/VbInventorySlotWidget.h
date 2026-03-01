// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbInventorySlotWidget.generated.h"

class UVbSlotObject;
class UVbInventoryComponent;
class UVbItemInstance;
class UVbItemToolTipWidget;
class UTextBlock;
class UImage;

UCLASS()
class UVbInventorySlotWidget : public UVbUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    FORCEINLINE UVbItemInstance* GetItemInstance() const { return ItemInstance; }

    void InitializeSlot(UVbItemInstance* InItemInstance);

private:
    UPROPERTY(EditDefaultsOnly, Category="ToolTip")
    TSubclassOf<UVbItemToolTipWidget> ItemToolTipWidgetClass;
    
    UPROPERTY(meta=(BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CountText;

    UPROPERTY()
    UVbItemInstance* ItemInstance;

    UPROPERTY()
    UVbSlotObject* SlotObject;

    UPROPERTY()
    UVbInventoryComponent* InventoryComponent;

    UPROPERTY()
    UVbEquipmentComponent* EquipmentComponent;

    void CreateToolTipWidget();
    void RequestUseItem() const;
    void RequestDropItem() const;
    void RequestSwapItems(const int32 FromIndex, const int32 ToIndex) const;
    void RequestEquipItem() const;
    void RequestUnequip(int32 EquipmentIndex) const;

public:
    int32 GetSlotIndex() const;
};