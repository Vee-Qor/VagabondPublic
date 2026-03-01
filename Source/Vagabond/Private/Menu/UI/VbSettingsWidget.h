// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbSettingsWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE(FOnSettingsClosedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSettingsApplyDelegate);

UCLASS()
class UVbSettingsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    FOnSettingsApplyDelegate OnSettingsApply;
    FOnSettingsClosedDelegate OnSettingsClosed;

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnCloseClicked();

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UButton* ApplyBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* ApplyText;

    UPROPERTY(meta=(BindWidget))
    UButton* CloseBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CloseText;
};