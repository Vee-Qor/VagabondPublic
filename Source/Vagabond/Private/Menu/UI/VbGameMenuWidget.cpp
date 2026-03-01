// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/UI/VbGameMenuWidget.h"

#include "VbConfirmWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/VbGameInstance.h"
#include "VbSettingsWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void UVbGameMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SettingsBtn && MainMenuBtn && ExitGameBtn)
    {
        SettingsBtn->OnClicked.AddDynamic(this, &UVbGameMenuWidget::OnSettingsClicked);
        MainMenuBtn->OnClicked.AddDynamic(this, &UVbGameMenuWidget::OnMainMenuClicked);
        ExitGameBtn->OnClicked.AddDynamic(this, &UVbGameMenuWidget::OnExitGameClicked);
    }

    if (SettingsWidget)
    {
        SettingsWidget->OnSettingsApply.AddUObject(this, &UVbGameMenuWidget::OnSettingsApply);
        SettingsWidget->OnSettingsClosed.AddUObject(this, &UVbGameMenuWidget::OnSettingsClosed);
    }

    if (ExitGameConfirmWidget)
    {
        ExitGameConfirmWidget->OnConfirm.AddUObject(this, &UVbGameMenuWidget::OnExitConfirm);
        ExitGameConfirmWidget->OnCancel.AddUObject(this, &UVbGameMenuWidget::OnExitCancel);
    }
}

void UVbGameMenuWidget::ToggleGameMenu()
{
    if (GetVisibility() == ESlateVisibility::Visible)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        SetFocusToGameOnly();
    }
    else
    {
        SetVisibility(ESlateVisibility::Visible);
        SetFocusToGameAndUI();
        SetActiveWidget(MainPanel);
    }
}

void UVbGameMenuWidget::SetActiveWidget(UWidget* Widget) const
{
    if (!WidgetSwitcher || !Widget) return;

    WidgetSwitcher->SetActiveWidget(Widget);
}

void UVbGameMenuWidget::OnSettingsClicked()
{
    SetActiveWidget(SettingsWidget);
}

void UVbGameMenuWidget::OnSettingsApply()
{
    //TODO::Implement when create the settings menu.
}

void UVbGameMenuWidget::OnSettingsClosed()
{
    SetActiveWidget(MainPanel);
}

void UVbGameMenuWidget::OnMainMenuClicked()
{
    UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    GameInstance->LeaveSession();
}

void UVbGameMenuWidget::OnExitGameClicked()
{
    SetActiveWidget(ExitGameConfirmWidget);
}

void UVbGameMenuWidget::OnExitConfirm()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UVbGameMenuWidget::OnExitCancel()
{
    SetActiveWidget(MainPanel);
}