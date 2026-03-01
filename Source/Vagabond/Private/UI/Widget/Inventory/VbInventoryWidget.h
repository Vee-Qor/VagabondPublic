// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbInventoryWidget.generated.h"

class UTileView;
class UVbInventoryComponent;

UCLASS()
class UVbInventoryWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void ToggleInventory();

private:
    UPROPERTY(meta=(BindWidget))
    UTileView* InventoryList;

    UPROPERTY()
    UVbInventoryComponent* InventoryComponent;

    void OnInventoryChanged();
};