// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/UI/VbMainMenuWidget.h"

#include "VbConfirmWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/HorizontalBox.h"
#include "Framework/VbGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "VbSettingsWidget.h"
#include "VbTeamSlotWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "VbLobbyWidget.h"
#include "Menu/VbMenuGameState.h"
#include "Menu/VbMenuPlayerController.h"

void UVbMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (LobbyBtn && SettingsBtn && ExitGameBtn)
    {
        LobbyBtn->OnClicked.AddDynamic(this, &UVbMainMenuWidget::OnLobbyClicked);
        SettingsBtn->OnClicked.AddDynamic(this, &UVbMainMenuWidget::OnSettingsClicked);
        ExitGameBtn->OnClicked.AddDynamic(this, &UVbMainMenuWidget::OnExitGameClicked);
    }

    if (SettingsWidget)
    {
        SettingsWidget->OnSettingsApply.AddUObject(this, &UVbMainMenuWidget::OnSettingsApply);
        SettingsWidget->OnSettingsClosed.AddUObject(this, &UVbMainMenuWidget::OnSettingsClosed);
    }

    if (ExitGameConfirmWidget)
    {
        ExitGameConfirmWidget->OnConfirm.AddUObject(this, &UVbMainMenuWidget::OnExitConfirm);
        ExitGameConfirmWidget->OnCancel.AddUObject(this, &UVbMainMenuWidget::OnExitCancel);
    }

    CreateTeamSlots();

    if (AVbMenuPlayerController* PlayerController = Cast<AVbMenuPlayerController>(GetOwningPlayer()))
    {
        PlayerController->OnPlayerStateReady.AddUObject(this, &UVbMainMenuWidget::OnPlayerStateReady);
    }

    if (const UWorld* World = GetWorld())
    {
        AVbMenuGameState* GameState = Cast<AVbMenuGameState>(World->GetGameState());
        if (!GameState) return;

        GameState->OnSessionPlayersChanged.AddUObject(this, &UVbMainMenuWidget::UpdateTeamSlots);
        UpdateTeamSlots();
    }
}

void UVbMainMenuWidget::SetActiveWidget(UWidget* Widget) const
{
    if (!WidgetSwitcher || !Widget) return;

    WidgetSwitcher->SetActiveWidget(Widget);
}

void UVbMainMenuWidget::CreateTeamSlots()
{
    const UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance || !TeamSlotsBox || !TeamSlotWidgetClass) return;

    TeamSlotsBox->ClearChildren();
    TeamSlots.Reset();

    for (int32 i = 0; i < GameInstance->GetMaxPlayers(); i++)
    {
        UVbTeamSlotWidget* TeamSlotWidget = CreateWidget<UVbTeamSlotWidget>(GetWorld(), TeamSlotWidgetClass);
        if (!TeamSlotWidget) continue;

        TeamSlotsBox->AddChild(TeamSlotWidget);
        TeamSlots.Add(TeamSlotWidget);
    }
}

void UVbMainMenuWidget::ClearTeamSlots()
{
    for (UVbTeamSlotWidget* TeamSlotWidget : TeamSlots)
    {
        if (!TeamSlotWidget) continue;

        TeamSlotWidget->ClearSlot();
    }
}

void UVbMainMenuWidget::UpdateTeamSlots()
{
    if (TeamSlots.Num() <= 0) return;
    
    const UWorld* World = GetWorld();
    if (!World) return;

    const AGameStateBase* GameState = World->GetGameState();
    if (!GameState) return;

    APlayerState* OwnerPlayerState = GetOwningPlayerState();
    if (!OwnerPlayerState) return;
    
    ClearTeamSlots();

    const int32 CenterIndex = TeamSlots.Num() / 2;
    TeamSlots[CenterIndex]->InitSlot(OwnerPlayerState);

    int32 SlotIndex = 0;

    for (APlayerState* PlayerState : GameState->PlayerArray)
    {
        if (!PlayerState || PlayerState == OwnerPlayerState) continue;
        
        while (SlotIndex == CenterIndex)
        {
            ++SlotIndex;
        }

        if (SlotIndex >= TeamSlots.Num()) break;

        TeamSlots[SlotIndex]->InitSlot(PlayerState);
        ++SlotIndex;
    }
}

void UVbMainMenuWidget::OnPlayerStateReady(APlayerState* PlayerState)
{
    UpdateTeamSlots();
}

void UVbMainMenuWidget::OnSettingsApply()
{
    //TODO::Implement when create the settings menu.
}

void UVbMainMenuWidget::OnSettingsClosed()
{
    SetActiveWidget(LobbyWidget);
}

void UVbMainMenuWidget::OnExitConfirm()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UVbMainMenuWidget::OnExitCancel()
{
    SetActiveWidget(LobbyWidget);
}

void UVbMainMenuWidget::OnLobbyClicked()
{
    SetActiveWidget(LobbyWidget);
}

void UVbMainMenuWidget::OnSettingsClicked()
{
    SetActiveWidget(SettingsWidget);
}

void UVbMainMenuWidget::OnExitGameClicked()
{
    SetActiveWidget(ExitGameConfirmWidget);
}