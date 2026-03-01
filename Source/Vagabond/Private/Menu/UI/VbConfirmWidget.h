// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbConfirmWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE(FOnConfirmDelegate);
DECLARE_MULTICAST_DELEGATE(FOnCancelDelegate);

UCLASS()
class UVbConfirmWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    FOnConfirmDelegate OnConfirm;
    FOnCancelDelegate OnCancel;

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnConfirmClicked();

    UFUNCTION()
    void OnCancelClicked();

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UButton* ConfirmBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* ConfirmText;

    UPROPERTY(meta=(BindWidget))
    UButton* CancelBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CancelText;
};