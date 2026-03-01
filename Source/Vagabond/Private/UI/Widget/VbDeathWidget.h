// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbUserWidget.h"
#include "VbDeathWidget.generated.h"

class UAbilitySystemComponent;
class UButton;
class UVbConfirmWidget;
class UOverlay;
class UTextBlock;
class UWidgetSwitcher;
struct FGameplayTag;

UCLASS()
class UVbDeathWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    void InitFromASC(UAbilitySystemComponent* ASC);

protected:
    virtual void NativeConstruct() override;

private:
    void OnDeadTagChanged(FGameplayTag Tag, int32 NewCount);

    UFUNCTION()
    void OnRespawnClicked();

    UFUNCTION()
    void OnMainMenuClicked();

    UFUNCTION()
    void OnConfirmMainMenu();

    UFUNCTION()
    void OnCancelMainMenu();

    UPROPERTY(meta=(BindWidget))
    UWidgetSwitcher* WidgetSwitcher;

    UPROPERTY(meta=(BindWidget))
    UOverlay* MainOverlay;

    UPROPERTY(meta=(BindWidget))
    UButton* RespawnBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* RespawnText;

    UPROPERTY(meta=(BindWidget))
    UButton* MainMenuBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* MainMenuText;

    UPROPERTY(meta=(BindWidget))
    UVbConfirmWidget* MainMenuConfirmWidget;
};