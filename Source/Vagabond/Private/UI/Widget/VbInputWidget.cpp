// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbInputWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UVbInputWidget::SetInput(UTexture2D* Texture, const FText& Text)
{
    if (!InputIcon || !InputText || !Texture) return;

    InputIcon->SetBrushFromTexture(Texture);
    InputText->SetText(Text);
}