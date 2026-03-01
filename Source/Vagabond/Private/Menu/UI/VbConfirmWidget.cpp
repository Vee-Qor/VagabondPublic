// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "VbConfirmWidget.h"

#include "Components/Button.h"

void UVbConfirmWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ConfirmBtn || !CancelBtn) return;
    ConfirmBtn->OnClicked.AddDynamic(this, &UVbConfirmWidget::OnConfirmClicked);
    CancelBtn->OnClicked.AddDynamic(this, &UVbConfirmWidget::OnCancelClicked);
}

void UVbConfirmWidget::OnConfirmClicked()
{
    OnConfirm.Broadcast();
}

void UVbConfirmWidget::OnCancelClicked()
{
    OnCancel.Broadcast();
}