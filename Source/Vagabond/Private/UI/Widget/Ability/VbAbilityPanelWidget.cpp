// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Ability/VbAbilityPanelWidget.h"

#include "VbAbilitySlotWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbGameplayTags.h"

void UVbAbilityPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ASC = Cast<UVbAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()));
    if (!ASC) return;

    ASC->RegisterGameplayTagEvent(VbGameplayTags::State_Combat_Armed).AddUObject(this, &UVbAbilityPanelWidget::OnArmedTagChanged);
    OnArmedTagChanged(FGameplayTag(), 0);

    ASC->OnAbilitiesChanged.AddUObject(this, &UVbAbilityPanelWidget::UpdateSlots);

    ClearSlots();
}

void UVbAbilityPanelWidget::OnArmedTagChanged(FGameplayTag Tag, int32 NewCount)
{
    SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UVbAbilityPanelWidget::UpdateSlots()
{
    ClearSlots();

    for (const auto& AbilitySpec : ASC->GetActivatableAbilities())
    {
        const UVbGameplayAbility* Ability = Cast<UVbGameplayAbility>(AbilitySpec.Ability);
        if (!Ability) continue;

        const FVbAbilityWidgetData* AbilityWidgetData = GetAbilityWidgetData(Ability->GetClass());
        if (!AbilityWidgetData) continue;

        const FGameplayTag& InputTag = Ability->GetInputTag();
        if (!InputTag.IsValid()) continue;

        if (InputTag == VbGameplayTags::Input_AbilityOne)
        {
            FirstSlot->InitializeFromWidgetData(*AbilityWidgetData);
            continue;
        }

        if (InputTag == VbGameplayTags::Input_AbilityTwo)
        {
            SecondSlot->InitializeFromWidgetData(*AbilityWidgetData);
            continue;
        }

        if (InputTag == VbGameplayTags::Input_AbilityThree)
        {
            ThirdSlot->InitializeFromWidgetData(*AbilityWidgetData);
            continue;
        }

        if (InputTag == VbGameplayTags::Input_AbilityFour)
        {
            FourthSlot->InitializeFromWidgetData(*AbilityWidgetData);
            continue;
        }
    }
}

void UVbAbilityPanelWidget::ClearSlots() const
{
    if (FirstSlot && SecondSlot && ThirdSlot && FourthSlot)
    {
        FirstSlot->ClearSlot();
        SecondSlot->ClearSlot();
        ThirdSlot->ClearSlot();
        FourthSlot->ClearSlot();
    }
}

const FVbAbilityWidgetData* UVbAbilityPanelWidget::GetAbilityWidgetData(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
    if (!AbilityDataRow) return nullptr;

    for (const TPair<FName, uint8*>& AbilityWidgetDataPair : AbilityDataRow->GetRowMap())
    {
        const FVbAbilityWidgetData* WidgetData = AbilityDataRow->FindRow<FVbAbilityWidgetData>(AbilityWidgetDataPair.Key, "");
        if (WidgetData->AbilityClass == AbilityClass)
        {
            return WidgetData;
        }
    }

    return nullptr;
}