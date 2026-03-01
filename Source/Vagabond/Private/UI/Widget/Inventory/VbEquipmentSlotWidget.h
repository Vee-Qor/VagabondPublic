// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbEquipmentSlotWidget.generated.h"

class UImage;
class UVbItemInstance;
class UVbItemToolTipWidget;

UCLASS()
class UVbEquipmentSlotWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativePreConstruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    FORCEINLINE const FGameplayTag& GetSlotTag() const { return SlotTag; }
    void SetItem(UVbItemInstance* Item);

private:
    UPROPERTY(meta=(BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta=(BindWidget))
    UImage* SlotIcon;

    // ==== Config ==== //
    UPROPERTY(EditAnywhere, Category="Slot")
    UTexture2D* SlotImage;

    UPROPERTY(EditAnywhere, Category="Slot", meta=(Categories="Equipment.Slot"))
    FGameplayTag SlotTag;

    UPROPERTY(EditDefaultsOnly, Category="ToolTip")
    TSubclassOf<UVbItemToolTipWidget> ItemToolTipWidgetClass;

    void CreateToolTipWidget();
    void RequestEquipItem(int32 InventoryIndex, int32 EquipmentIndex);
    void RequestSwapEquipment(int32 FromIndex, int32 ToIndex);
    void RequestUnequipItem();

    UPROPERTY()
    UVbItemInstance* ItemInstance;

    UPROPERTY()
    UVbEquipmentComponent* EquipmentComponent;
};