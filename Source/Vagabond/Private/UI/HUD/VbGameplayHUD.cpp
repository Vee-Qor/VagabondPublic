// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/HUD/VbGameplayHUD.h"

#include "AbilitySystem/VbAttributeSet.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Menu/UI/VbGameMenuWidget.h"
#include "UI/Widget/VbDeathWidget.h"
#include "UI/Widget/VbEnemyOverlayWidget.h"
#include "UI/Widget/Inventory/VbEquipmentWidget.h"
#include "UI/Widget/Inventory/VbInventoryWidget.h"
#include "UI/Widget/VbPlayerHUDWidget.h"
#include "UI/Widget/VbValueGaugeWidget.h"
#include "UI/Widget/VbWinWidget.h"

void AVbGameplayHUD::CreateHUDAndInitializeWithASC(UVbAbilitySystemComponent* ASC)
{
    if (!PlayerHUDWidgetClass) return;

    PlayerHUDWidget = CreateWidget<UVbPlayerHUDWidget>(GetOwningPlayerController(), PlayerHUDWidgetClass);
    if (!PlayerHUDWidget) return;

    PlayerHUDWidget->AddToViewport();

    UVbValueGaugeWidget* HealthGaugeWidget = PlayerHUDWidget->GetHealthGaugeWidget();
    if (!HealthGaugeWidget || !ASC) return;

    HealthGaugeWidget->InitializeWithAttribute(ASC, UVbAttributeSet::GetHealthAttribute(), UVbAttributeSet::GetMaxHealthAttribute());

    UVbValueGaugeWidget* ResourceGaugeWidget = PlayerHUDWidget->GetResourceGaugeWidget();
    if (!ResourceGaugeWidget) return;

    ResourceGaugeWidget->InitializeWithAttribute(ASC, UVbAttributeSet::GetResourceAttribute(), UVbAttributeSet::GetMaxResourceAttribute());

    UVbDeathWidget* DeathWidget = PlayerHUDWidget->GetDeathWidget();
    if (!DeathWidget) return;

    DeathWidget->InitFromASC(ASC);
}

void AVbGameplayHUD::ToggleInventory() const
{
    if (!PlayerHUDWidget) return;

    UVbInventoryWidget* InventoryWidget = PlayerHUDWidget->GetInventoryWidget();
    if (!InventoryWidget) return;

    InventoryWidget->ToggleInventory();
}

void AVbGameplayHUD::ToggleEquipment() const
{
    if (!PlayerHUDWidget) return;

    UVbEquipmentWidget* EquipmentWidget = PlayerHUDWidget->GetEquipmentWidget();
    if (!EquipmentWidget) return;

    EquipmentWidget->ToggleEquipment();
}

void AVbGameplayHUD::ToggleGameMenu() const
{
    if (!PlayerHUDWidget) return;

    UVbGameMenuWidget* GameMenuWidget = PlayerHUDWidget->GetGameMenuWidget();
    if (!GameMenuWidget) return;

    GameMenuWidget->ToggleGameMenu();
}

void AVbGameplayHUD::InitializeOnScreenOverlay(UAbilitySystemComponent* ASC) const
{
    if (!PlayerHUDWidget) return;

    UVbEnemyOverlayWidget* OnScreenOverlayWidget = PlayerHUDWidget->GetEnemyOverlayWidget();
    if (!OnScreenOverlayWidget) return;

    OnScreenOverlayWidget->InitializeOverlay(ASC, EVbOverlayMode::OnScreen);
}

void AVbGameplayHUD::DisableOnScreenOverlay() const
{
    if (!PlayerHUDWidget) return;

    UVbEnemyOverlayWidget* OnScreenOverlayWidget = PlayerHUDWidget->GetEnemyOverlayWidget();
    if (!OnScreenOverlayWidget) return;

    OnScreenOverlayWidget->HideOverlay();
}