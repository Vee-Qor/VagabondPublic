// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/UI/VbTeamSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/VbGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Player/VbPlayerState.h"

void UVbTeamSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!SlotBtn) return;
    SlotBtn->OnClicked.AddDynamic(this, &UVbTeamSlotWidget::OnSlotClicked);
}

void UVbTeamSlotWidget::InitSlot(APlayerState* InPlayerState)
{
    if (!InPlayerState) return;

    PlayerState = Cast<AVbPlayerState>(InPlayerState);
    if (!PlayerState) return;

    PlayerState->OnNameChanged.AddUObject(this, &UVbTeamSlotWidget::OnNameChanged);
    OnNameChanged(PlayerState->GetPlayerName());

    PlayerState->OnReadyChanged.AddUObject(this, &UVbTeamSlotWidget::OnReadyChanged);
    OnReadyChanged(PlayerState->IsReady());
}

void UVbTeamSlotWidget::ClearSlot()
{
    UnbindFromPlayerState();

    if (!NameText || !AvatarIcon || !DefaultIcon) return;

    NameText->SetText(FText::GetEmpty());
    AvatarIcon->SetBrushFromTexture(DefaultIcon);
}

void UVbTeamSlotWidget::SetSteamAvatar() const
{
    if (!PlayerState || !AvatarIcon) return;

    const UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    AvatarIcon->SetBrushFromTexture(GameInstance->GetSteamAvatar(PlayerState->GetUniqueId()));
}

void UVbTeamSlotWidget::UnbindFromPlayerState()
{
    if (!PlayerState) return;

    PlayerState->OnNameChanged.RemoveAll(this);
    PlayerState->OnReadyChanged.RemoveAll(this);
    PlayerState = nullptr;
}

void UVbTeamSlotWidget::OnSlotClicked()
{
    if (PlayerState) return;

    const UVbGameInstance* GameInstance = Cast<UVbGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    GameInstance->OpenSteamInviteOverlay();
}

void UVbTeamSlotWidget::OnNameChanged(const FString& Name)
{
    SetSteamAvatar();

    if (!NameText) return;
    NameText->SetText(FText::FromString(Name));
}

void UVbTeamSlotWidget::OnReadyChanged(bool bReady)
{
    if (!NameText || !SlotBtn) return;

    NameText->SetColorAndOpacity(bReady ? ReadyColor : NotReadyColor);

    FButtonStyle ButtonStyle = SlotBtn->GetStyle();
    ButtonStyle.Normal.OutlineSettings.Color = bReady ? ReadyColor : NotReadyColor;
    SlotBtn->SetStyle(ButtonStyle);
    SlotBtn->SynchronizeProperties();
}