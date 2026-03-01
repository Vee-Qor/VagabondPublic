// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/VbMenuHUD.h"

#include "UI/VbMainMenuWidget.h"

void AVbMenuHUD::BeginPlay()
{
    Super::BeginPlay();

    CreateMainMenuWidget();
}

void AVbMenuHUD::CreateMainMenuWidget() const
{
    if (!MainMenuWidgetClass) return;

    UVbMainMenuWidget* MainMenuWidget = CreateWidget<UVbMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
    if (!MainMenuWidget) return;

    MainMenuWidget->AddToViewport();
}