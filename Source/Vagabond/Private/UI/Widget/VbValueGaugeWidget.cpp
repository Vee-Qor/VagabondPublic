// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbValueGaugeWidget.h"

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UVbValueGaugeWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    NumberFormattingOptions.MaximumFractionalDigits = 0;

    if (ValueBar)
    {
        BarMaterial = ValueBar->GetDynamicMaterial();
        if (!BarMaterial) return;

        BarMaterial->SetVectorParameterValue(MainColorMatParamName, MainColor);
        BarMaterial->SetVectorParameterValue(DelayedColorMatParamName, DelayedColor);
        BarMaterial->SetVectorParameterValue(BackgroundColorMatParamName, BackgroundColor);
    }

    if (SizeBox)
    {
        SizeBox->SetWidthOverride(BoxSizeX);
        SizeBox->SetHeightOverride(BoxSizeY);
    }

    if (ValueText)
    {
        FSlateFontInfo FontInfo = ValueText->GetFont();
        FontInfo.Size = FontSize;
        ValueText->SetFont(FontInfo);
    }
}

void UVbValueGaugeWidget::NativeDestruct()
{
    StopDelayedTimer();

    Super::NativeDestruct();
}

void UVbValueGaugeWidget::InitializeWithAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
    if (!ASC || !Attribute.IsValid() || !MaxAttribute.IsValid()) return;

    bool bFound = false;
    CachedValue = ASC->GetGameplayAttributeValue(Attribute, bFound);
    CachedMaxValue = ASC->GetGameplayAttributeValue(MaxAttribute, bFound);

    UpdateDisplay();

    ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UVbValueGaugeWidget::AttributeChanged);
    ASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UVbValueGaugeWidget::MaxAttributeChanged);
}

void UVbValueGaugeWidget::AttributeChanged(const FOnAttributeChangeData& ChangeData)
{
    CachedValue = ChangeData.NewValue;
    UpdateDisplay();
}

void UVbValueGaugeWidget::MaxAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
    CachedMaxValue = ChangeData.NewValue;
    UpdateDisplay();
}

void UVbValueGaugeWidget::UpdateDisplay()
{
    if (!ValueText || !BarMaterial) return;

    ValueText->SetText(FText::AsNumber(CachedValue, &NumberFormattingOptions));

    CachedMaxValue = FMath::Max(KINDA_SMALL_NUMBER, CachedMaxValue);
    MainPercent = CachedValue / CachedMaxValue;
    BarMaterial->SetScalarParameterValue(MainPercentMatParamName, MainPercent);

    if (MainPercent >= DelayedPercent)
    {
        DelayedPercent = MainPercent;
        BarMaterial->SetScalarParameterValue(DelayedPercentMatParamName, DelayedPercent);
        StopDelayedTimer();
        return;
    }

    BarMaterial->SetScalarParameterValue(DelayedPercentMatParamName, DelayedPercent);
    StartDelayedTimer();
}

void UVbValueGaugeWidget::StartDelayedTimer()
{
    if (const UWorld* World = GetWorld())
    {
        if (World->GetTimerManager().IsTimerActive(DelayedUpdateTimerHandle)) return;

        World->GetTimerManager().SetTimer(DelayedUpdateTimerHandle, this, &UVbValueGaugeWidget::UpdateDelayedPercent, DelayedUpdateInterval, true);
    }
}

void UVbValueGaugeWidget::StopDelayedTimer()
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(DelayedUpdateTimerHandle);
    }
}

void UVbValueGaugeWidget::UpdateDelayedPercent()
{
    if (!BarMaterial || FMath::IsNearlyEqual(DelayedPercent, MainPercent))
    {
        StopDelayedTimer();
        return;
    }

    DelayedPercent = FMath::FInterpTo(DelayedPercent, MainPercent, DelayedUpdateInterval, DelayedInterpSpeed);
    BarMaterial->SetScalarParameterValue(DelayedPercentMatParamName, DelayedPercent);
}