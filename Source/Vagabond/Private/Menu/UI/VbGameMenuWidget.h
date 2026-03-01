// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbGameMenuWidget.generated.h"

class UButton;
class UCanvasPanel;
class UVbConfirmWidget;
class UTextBlock;
class UVbSettingsWidget;
class UWidgetSwitcher;

UCLASS()
class UVbGameMenuWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    void ToggleGameMenu();

protected:
    virtual void NativeConstruct() override;

private:
    void SetActiveWidget(UWidget* Widget) const;
    
    void OnSettingsApply();
    void OnSettingsClosed();
    
    void OnExitConfirm();
    void OnExitCancel();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnMainMenuClicked();

    UFUNCTION()
    void OnExitGameClicked();

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta=(BindWidget))
    UCanvasPanel* MainPanel;

    UPROPERTY(meta=(BindWidget))
    UVbSettingsWidget* SettingsWidget;

    UPROPERTY(meta=(BindWidget))
    UButton* SettingsBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* SettingsText;

    UPROPERTY(meta=(BindWidget))
    UButton* MainMenuBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* MainMenuText;

    UPROPERTY(meta=(BindWidget))
    UButton* ExitGameBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* ExitGameText;

    UPROPERTY(meta=(BindWidget))
    UVbConfirmWidget* ExitGameConfirmWidget;
};