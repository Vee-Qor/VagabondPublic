// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/Ability/VbAbilitySlotWidget.h"

#include "VbAbilityToolTip.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/VbGameplayAbility.h"
#include "VbCoreTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/VbFunctionLibrary.h"
#include "UI/Widget/VbInputWidget.h"

void UVbAbilitySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CooldownFormattingOptions.SetMaximumFractionalDigits(0);

    if (InputWidget)
    {
        InputWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UVbAbilitySlotWidget::InitializeFromWidgetData(const FVbAbilityWidgetData& InAbilityWidgetData)
{
    if (!Icon) return;

    AbilityWidgetData = InAbilityWidgetData;

    Icon->SetBrushFromTexture(AbilityWidgetData.Icon);
    Icon->SetVisibility(ESlateVisibility::Visible);

    CreateToolTipWidget();

    ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (!ASC) return;

    ASC->AbilityActivatedCallbacks.AddUObject(this, &UVbAbilitySlotWidget::OnAbilityActivated);

    const FVbAbilityParamsRow* AbilityParamsRow = UVbFunctionLibrary::GetAbilityParamsRow(GetWorld(), AbilityWidgetData.AbilityClass);
    if (!AbilityParamsRow) return;

    CooldownTag = AbilityParamsRow->CooldownTag;

    ASC->RegisterGameplayTagEvent(CooldownTag).AddUObject(this, &UVbAbilitySlotWidget::OnStartCooldown);
    if (ASC->GetTagCount(CooldownTag) > 0)
    {
        OnStartCooldown(CooldownTag, 1);
    }

    InitInput();
}

void UVbAbilitySlotWidget::InitInput()
{
    const UDataTable* KeyIconTable = UVbFunctionLibrary::GetKeyIconRow(GetWorld());
    if (!KeyIconTable) return;

    const UVbGameplayAbility* AbilityCDO = AbilityWidgetData.AbilityClass->GetDefaultObject<UVbGameplayAbility>();
    if (!AbilityCDO) return;

    const FKey* KeyPtr = UVbFunctionLibrary::GetKeyForInputTag(GetOwningPlayerPawn(), AbilityCDO->GetInputTag());
    if (!KeyPtr) return;

    for (const auto& KetIconPair : KeyIconTable->GetRowMap())
    {
        const FVbKeyIconRow* Row = KeyIconTable->FindRow<FVbKeyIconRow>(KetIconPair.Key, "");
        if (Row->Key != *KeyPtr) continue;

        InputWidget->SetInput(Row->Texture.LoadSynchronous(), Row->Key.GetDisplayName());
        InputWidget->SetVisibility(ESlateVisibility::Visible);
        return;
    }
}

void UVbAbilitySlotWidget::ClearSlot()
{
    if (!Icon || !InputWidget) return;

    Icon->SetVisibility(ESlateVisibility::Collapsed);
    StopCooldown();

    GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);

    if (ASC)
    {
        ASC->RegisterGameplayTagEvent(CooldownTag).RemoveAll(this);
        ASC->AbilityActivatedCallbacks.RemoveAll(this);
    }

    InputWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UVbAbilitySlotWidget::CreateToolTipWidget()
{
    if (!AbilityToolTipClass) return;

    UVbAbilityToolTip* AbilityToolTip = CreateWidget<UVbAbilityToolTip>(this, AbilityToolTipClass);
    if (!AbilityToolTip) return;

    const FVbAbilityParamsRow* AbilityParamsRow = UVbFunctionLibrary::GetAbilityParamsRow(GetWorld(), AbilityWidgetData.AbilityClass);
    if (!AbilityParamsRow) return;

    AbilityToolTip->SetInfo(AbilityWidgetData.Icon, AbilityWidgetData.Name, AbilityWidgetData.Description, AbilityParamsRow->Damage.GetValue(),
        AbilityParamsRow->Cooldown.GetValue(), AbilityParamsRow->Cost.GetValue());
    SetToolTip(AbilityToolTip);
}

void UVbAbilitySlotWidget::OnAbilityActivated(UGameplayAbility* ActivatedAbility)
{
    if (!ActivatedAbility || ActivatedAbility->GetClass() != AbilityWidgetData.AbilityClass) return;

    PlayAnimation(PulseAnimation);
}

void UVbAbilitySlotWidget::OnStartCooldown(FGameplayTag Tag, int32 NewCount)
{
    const UWorld* World = GetWorld();
    if (!World) return;

    if (NewCount <= 0)
    {
        StopCooldown();
        return;
    }

    if (!World->GetTimerManager().IsTimerActive(CooldownTimerHandle))
    {
        World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UVbAbilitySlotWidget::UpdateCooldown, CooldownUpdateInterval, true);
    }

    UpdateCooldown();
}

void UVbAbilitySlotWidget::UpdateCooldown()
{
    if (!ASC || !CooldownTag.IsValid())
    {
        StopCooldown();
        return;
    }

    float TimeRemaining = 0.0f;
    float Duration = 0.0f;

    const FGameplayEffectQuery EffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
    TArray<TPair<float, float>> RemainingAndDurations = ASC->GetActiveEffectsTimeRemainingAndDuration(EffectQuery);

    if (RemainingAndDurations.Num() > 0)
    {
        int32 BestIndex = 0;
        float LongestRemaining = RemainingAndDurations[0].Key;

        for (int32 i = 1; i < RemainingAndDurations.Num(); ++i)
        {
            if (RemainingAndDurations[i].Key > LongestRemaining)
            {
                LongestRemaining = RemainingAndDurations[i].Key;
                BestIndex = i;
            }
        }

        TimeRemaining = RemainingAndDurations[BestIndex].Key;
        Duration = RemainingAndDurations[BestIndex].Value;
    }

    if (CooldownIcon)
    {
        const float Percent = Duration > 0.0f ? TimeRemaining / Duration : 0.0f;
        if (UMaterialInstanceDynamic* MID = CooldownIcon->GetDynamicMaterial())
        {
            MID->SetScalarParameterValue(CooldownMatParam, Percent);
        }
    }

    if (TimeRemaining > 0.0f)
    {
        if (CooldownText)
        {
            CooldownText->SetText(FText::AsNumber(TimeRemaining, &CooldownFormattingOptions));
            CooldownText->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else
    {
        StopCooldown();
    }
}

void UVbAbilitySlotWidget::StopCooldown()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    if (CooldownTimerHandle.IsValid())
    {
        World->GetTimerManager().ClearTimer(CooldownTimerHandle);
    }

    if (CooldownText)
    {
        CooldownText->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (CooldownIcon)
    {
        CooldownIcon->GetDynamicMaterial()->SetScalarParameterValue(CooldownMatParam, 0.0f);
    }
}