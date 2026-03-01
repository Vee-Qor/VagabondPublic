// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget//VbLevelWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UVbLevelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (!OwnerASC) return;

    OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetExperienceAttribute()).AddUObject(this, &UVbLevelWidget::UpdateLevel);
    OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetPrevExperienceAttribute()).AddUObject(this, &UVbLevelWidget::UpdateLevel);
    OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetNextExperienceAttribute()).AddUObject(this, &UVbLevelWidget::UpdateLevel);
    OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetLevelAttribute()).AddUObject(this, &UVbLevelWidget::UpdateLevel);
    UpdateLevel(FOnAttributeChangeData());
}

void UVbLevelWidget::UpdateLevel(const FOnAttributeChangeData& ChangeData)
{
    bool bFound;

    const float Experience = OwnerASC->GetGameplayAttributeValue(UVbAttributeSet::GetExperienceAttribute(), bFound);
    if (!bFound) return;

    const float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UVbAttributeSet::GetPrevExperienceAttribute(), bFound);
    if (!bFound) return;

    const float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UVbAttributeSet::GetNextExperienceAttribute(), bFound);
    if (!bFound) return;

    const float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UVbAttributeSet::GetLevelAttribute(), bFound);
    if (!bFound) return;

    LevelText->SetText(FText::AsNumber(CurrentLevel));

    const float Progress = Experience - PrevLevelExperience;
    const float LevelExperienceAmount = NextLevelExperience - PrevLevelExperience;

    float Percent = Progress / LevelExperienceAmount;

    if (NextLevelExperience == 0.0f)
    {
        Percent = 1.0f;
    }

    LevelProgressIcon->GetDynamicMaterial()->SetScalarParameterValue(PercentMatParamName, Percent);
}