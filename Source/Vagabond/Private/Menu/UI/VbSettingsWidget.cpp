// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/UI/VbSettingsWidget.h"

#include "Components/Button.h"

void UVbSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ApplyBtn || !CloseBtn) return;
    ApplyBtn->OnClicked.AddDynamic(this, &UVbSettingsWidget::OnApplyClicked);
    CloseBtn->OnClicked.AddDynamic(this, &UVbSettingsWidget::OnCloseClicked);
}

void UVbSettingsWidget::OnApplyClicked()
{
    //TODO::Implement when create the settings menu.
}

void UVbSettingsWidget::OnCloseClicked()
{
    OnSettingsClosed.Broadcast();
}