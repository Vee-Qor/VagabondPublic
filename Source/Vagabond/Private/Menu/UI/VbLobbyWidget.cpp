// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/UI/VbLobbyWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Framework/VbGameInstance.h"
#include "VbTeamSlotWidget.h"
#include "Components/SizeBox.h"
#include "Menu/VbMenuGameState.h"
#include "Menu/VbMenuPlayerController.h"
#include "Player/VbPlayerState.h"

void UVbLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    NumberFormattingOptions.MaximumFractionalDigits = 0;

    if (ReadyBtn && LeaveBtn)
    {
        ReadyBtn->OnClicked.AddDynamic(this, &UVbLobbyWidget::OnReadyClicked);
        LeaveBtn->OnClicked.AddDynamic(this, &UVbLobbyWidget::OnLeaveClicked);
    }

    if (const UWorld* World = GetWorld())
    {
        AVbMenuGameState* GameState = Cast<AVbMenuGameState>(World->GetGameState());
        if (!GameState) return;

        GameState->OnSessionPlayersChanged.AddUObject(this, &UVbLobbyWidget::UpdateTeamSlots);
        UpdateTeamSlots();

        GameState->OnSessionStartTimer.AddUObject(this, &UVbLobbyWidget::OnSessionStartTimer);
    }

    if (AVbPlayerState* PlayerState = Cast<AVbPlayerState>(GetOwningPlayerState()))
    {
        OnPlayerStateReady(PlayerState);
    }
    else
    {
        if (AVbMenuPlayerController* PlayerController = Cast<AVbMenuPlayerController>(GetOwningPlayer()))
        {
            PlayerController->OnPlayerStateReady.AddUObject(this, &UVbLobbyWidget::OnPlayerStateReady);
        }
    }
}

void UVbLobbyWidget::UpdateTeamSlots()
{
    if (!FirstSizeBox || !SecondSizeBox || !ThirdSizeBox) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    const AGameStateBase* GameState = World->GetGameState();
    if (!GameState) return;

    FirstSizeBox->ClearChildren();
    SecondSizeBox->ClearChildren();
    ThirdSizeBox->ClearChildren();

    for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
    {
        APlayerState* PlayerState = GameState->PlayerArray[i];
        if (!PlayerState) continue;

        if (PlayerState == GetOwningPlayerState())
        {
            CreateTeamSlotWidget(FirstSizeBox, PlayerState);
            continue;
        }

        if (!SecondSizeBox->HasAnyChildren())
        {
            CreateTeamSlotWidget(SecondSizeBox, PlayerState);
            continue;
        }

        if (!ThirdSizeBox->HasAnyChildren())
        {
            CreateTeamSlotWidget(ThirdSizeBox, PlayerState);
            continue;
        }
    }

    SetLeaveVisibility(GameState->PlayerArray.Num() > 1);
}

void UVbLobbyWidget::OnPlayerStateReady(APlayerState* InPlayerState)
{
    AVbPlayerState* PlayerState = Cast<AVbPlayerState>(InPlayerState);
    if (!PlayerState) return;

    PlayerState->OnReadyChanged.RemoveAll(this);
    PlayerState->OnReadyChanged.AddUObject(this, &UVbLobbyWidget::OnReadyChanged);
    OnReadyChanged(PlayerState->IsReady());
    UpdateTeamSlots();
}

void UVbLobbyWidget::OnSessionStartTimer(bool bIsActive)
{
    if (!StartDelayText) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    const AVbMenuGameState* GameState = Cast<AVbMenuGameState>(World->GetGameState());
    if (!GameState) return;

    if (!bIsActive)
    {
        World->GetTimerManager().ClearTimer(MatchCountdownTimerHandle);
        StartDelayText->SetText(FText::AsNumber(GameState->GetMatchStartDelay(), &NumberFormattingOptions));
        return;
    }

    World->GetTimerManager().SetTimer(MatchCountdownTimerHandle, this, &UVbLobbyWidget::UpdateSessionRunText, StartDelayUpdateInterval, true);
}

void UVbLobbyWidget::UpdateSessionRunText()
{
    const UWorld* World = GetWorld();
    if (!World || !StartDelayText) return;

    const AVbMenuGameState* GameState = Cast<AVbMenuGameState>(World->GetGameState());
    if (!GameState) return;

    const float EndTime = GameState->GetMatchStartTimeServer();
    if (EndTime <= 0.0f)
    {
        World->GetTimerManager().ClearTimer(MatchCountdownTimerHandle);
        StartDelayText->SetText(FText::AsNumber(GameState->GetMatchStartDelay(), &NumberFormattingOptions));
        return;
    }

    const float CurrentTime = GameState->GetServerWorldTimeSeconds();
    const float RemainingTime = FMath::Max(0.0f, EndTime - CurrentTime);

    StartDelayText->SetText(FText::AsNumber(RemainingTime, &NumberFormattingOptions));

    if (RemainingTime <= 0.0f)
    {
        World->GetTimerManager().ClearTimer(MatchCountdownTimerHandle);
    }
}

void UVbLobbyWidget::CreateTeamSlotWidget(USizeBox* SizeBox, APlayerState* PlayerState) const
{
    if (!SizeBox || !PlayerState || !TeamSlotWidgetClass) return;

    UVbTeamSlotWidget* TeamSlotWidget = CreateWidget<UVbTeamSlotWidget>(GetWorld(), TeamSlotWidgetClass);
    if (!TeamSlotWidget) return;

    TeamSlotWidget->InitSlot(PlayerState);
    SizeBox->AddChild(TeamSlotWidget);
}

void UVbLobbyWidget::SetLeaveVisibility(bool bShow)
{
    if (!LeaveBtn || !LeaveText) return;

    const ESlateVisibility NewVisibility = bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
    LeaveBtn->SetVisibility(NewVisibility);
    LeaveText->SetVisibility(NewVisibility);
}

void UVbLobbyWidget::OnReadyClicked()
{
    AVbPlayerState* PlayerState = Cast<AVbPlayerState>(GetOwningPlayerState());
    if (!PlayerState) return;

    PlayerState->Server_ToggleReady();
}

void UVbLobbyWidget::OnLeaveClicked()
{
    UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    GameInstance->LeaveSession();
}

void UVbLobbyWidget::OnReadyChanged(bool bReady)
{
    if (!ReadyText) return;

    ReadyText->SetColorAndOpacity(bReady ? ReadyColor : NotReadyColor);
}