// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbUserWidget.h"

#include "ItemSystem/VbEquipmentComponent.h"
#include "ItemSystem/VbInventoryComponent.h"

void UVbUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerController = GetOwningPlayer();
}

UVbEquipmentComponent* UVbUserWidget::GetEquipmentComponent() const
{
    const APawn* Pawn = GetOwningPlayerPawn();

    return Pawn ? Pawn->FindComponentByClass<UVbEquipmentComponent>() : nullptr;
}

UVbInventoryComponent* UVbUserWidget::GetInventoryComponent() const
{
    const APawn* Pawn = GetOwningPlayerPawn();

    return Pawn ? Pawn->FindComponentByClass<UVbInventoryComponent>() : nullptr;
}

void UVbUserWidget::SetShowMouseCursor(bool bShow) const
{
    if (PlayerController)
    {
        PlayerController->SetShowMouseCursor(bShow);
    }
}

void UVbUserWidget::SetInputEnabled(bool bEnable) const
{
    if (PlayerController)
    {
        if (bEnable)
        {
            PlayerController->EnableInput(PlayerController);
        }
        else
        {
            PlayerController->DisableInput(PlayerController);
        }
    }
}

void UVbUserWidget::SetFocusToGameAndUI() const
{
    if (PlayerController)
    {
        FInputModeGameAndUI GameAndUI;
        GameAndUI.SetHideCursorDuringCapture(false);
        PlayerController->SetInputMode(GameAndUI);
    }
}

void UVbUserWidget::SetFocusToGameOnly() const
{
    if (PlayerController)
    {
        FInputModeGameOnly GameOnly;
        PlayerController->SetInputMode(GameOnly);
    }
}