// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbMainMenuWidget.generated.h"

class UVbTeamSlotWidget;
class UHorizontalBox;
class UButton;
class UVbConfirmWidget;
class UImage;
class UTextBlock;
class UVbLevelWidget;
class UVbLobbyWidget;
class UVbMenuWidget;
class UVbSettingsWidget;
class UWidgetSwitcher;

UCLASS()
class UVbMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void SetActiveWidget(UWidget* Widget) const;

    void CreateTeamSlots();
    void ClearTeamSlots();
    void UpdateTeamSlots();

    void OnSettingsApply();
    void OnSettingsClosed();

    void OnExitConfirm();
    void OnExitCancel();

    void OnPlayerStateReady(APlayerState* PlayerState);

    UFUNCTION()
    void OnLobbyClicked();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnExitGameClicked();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UVbTeamSlotWidget> TeamSlotWidgetClass;

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta=(BindWidget))
    UHorizontalBox* TeamSlotsBox;

    UPROPERTY(meta=(BindWidget))
    UVbLobbyWidget* LobbyWidget;

    UPROPERTY(meta=(BindWidget))
    UVbSettingsWidget* SettingsWidget;

    UPROPERTY(meta=(BindWidget))
    UButton* LobbyBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* LobbyText;

    UPROPERTY(meta=(BindWidget))
    UButton* SettingsBtn;

    UPROPERTY(meta=(BindWidget))
    UImage* SettingsIcon;

    UPROPERTY(meta=(BindWidget))
    UButton* ExitGameBtn;

    UPROPERTY(meta=(BindWidget))
    UImage* ExitGameIcon;

    UPROPERTY(meta=(BindWidget))
    UVbConfirmWidget* ExitGameConfirmWidget;

    // ==== Runtime ==== //
    UPROPERTY()
    TArray<UVbTeamSlotWidget*> TeamSlots;
};