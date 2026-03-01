// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbPlayerHUDWidget.h"

#include "VbDeathWidget.h"
#include "VbEnemyOverlayWidget.h"
#include "Inventory/VbEquipmentWidget.h"
#include "Inventory/VbInventoryWidget.h"
#include "Menu/UI/VbGameMenuWidget.h"
#include "VbWinWidget.h"

void UVbPlayerHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (InventoryWidget) InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (EquipmentWidget) EquipmentWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (GameMenuWidget) GameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (EnemyOverlayWidget) EnemyOverlayWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (DeathWidget) DeathWidget->SetVisibility(ESlateVisibility::Collapsed);
    if (WinWidget) WinWidget->SetVisibility(ESlateVisibility::Collapsed);
}