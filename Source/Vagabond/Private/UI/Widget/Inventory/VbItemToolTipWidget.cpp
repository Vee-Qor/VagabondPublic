// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Inventory/VbItemToolTipWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UVbItemToolTipWidget::SetInfo(UTexture2D* Texture, const FText& Name, const FText& Description)
{
    if (!Icon || !NameText || !DescriptionText) return;

    Icon->SetBrushFromTexture(Texture);
    NameText->SetText(Name);
    DescriptionText->SetText(Description);
}