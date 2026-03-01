// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbDeathWidget.h"

#include "AbilitySystemComponent.h"
#include "Character/VbCharacter.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/VbGameInstance.h"
#include "VbGameplayTags.h"
#include "Menu/UI/VbConfirmWidget.h"

void UVbDeathWidget::InitFromASC(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    ASC->RegisterGameplayTagEvent(VbGameplayTags::State_Dead).AddUObject(this, &UVbDeathWidget::OnDeadTagChanged);
}

void UVbDeathWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RespawnBtn && MainMenuBtn)
    {
        RespawnBtn->OnClicked.AddDynamic(this, &UVbDeathWidget::OnRespawnClicked);
        MainMenuBtn->OnClicked.AddDynamic(this, &UVbDeathWidget::OnMainMenuClicked);
    }

    if (MainMenuConfirmWidget)
    {
        MainMenuConfirmWidget->OnConfirm.AddUObject(this, &UVbDeathWidget::OnConfirmMainMenu);
        MainMenuConfirmWidget->OnCancel.AddUObject(this, &UVbDeathWidget::OnCancelMainMenu);
    }
}


void UVbDeathWidget::OnDeadTagChanged(FGameplayTag Tag, int32 NewCount)
{
    if (NewCount == 0)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        SetFocusToGameOnly();
        SetShowMouseCursor(false);
    }
    else
    {
        SetVisibility(ESlateVisibility::Visible);
        SetFocusToGameAndUI();
        SetShowMouseCursor(true);
    }
}

void UVbDeathWidget::OnRespawnClicked()
{
    AVbCharacter* Character = Cast<AVbCharacter>(GetOwningPlayerPawn());
    if (!Character) return;

    Character->Server_RequestRespawn();
}

void UVbDeathWidget::OnMainMenuClicked()
{
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidget(MainMenuConfirmWidget);
    }
}

void UVbDeathWidget::OnConfirmMainMenu()
{
    UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    GameInstance->LeaveSession();
}

void UVbDeathWidget::OnCancelMainMenu()
{
    if (WidgetSwitcher)
    {
        WidgetSwitcher->SetActiveWidget(MainOverlay);
    }
}