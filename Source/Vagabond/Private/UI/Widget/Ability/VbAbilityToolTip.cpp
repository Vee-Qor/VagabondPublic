// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Ability/VbAbilityToolTip.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UVbAbilityToolTip::SetInfo(UTexture2D* Texture, const FText& Name, const FText& Description, float Damage, float Cooldown, float Cost)
{
    if (!Icon || !NameText || !DescriptionText || !DamageValueText || !CooldownValueText || !CostValueText) return;

    Icon->SetBrushFromTexture(Texture);
    NameText->SetText(Name);
    DescriptionText->SetText(Description);
    DamageValueText->SetText(FText::AsNumber(Damage));
    CooldownValueText->SetText(FText::AsNumber(Cooldown));
    CostValueText->SetText(FText::AsNumber(Cost));
}