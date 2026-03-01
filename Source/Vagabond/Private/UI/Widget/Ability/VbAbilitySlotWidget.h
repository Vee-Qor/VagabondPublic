// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "VbAbilitySlotWidget.generated.h"

class UVbAbilityToolTip;
class UAbilitySystemComponent;
class UGameplayAbility;
class UVbGameplayAbility;
class UImage;
class UVbInputWidget;
class UTextBlock;

USTRUCT(BlueprintType)
struct FVbAbilityWidgetData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UVbGameplayAbility> AbilityClass;

    UPROPERTY(EditDefaultsOnly)
    FText Name;

    UPROPERTY(EditDefaultsOnly)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditDefaultsOnly)
    FText Description;
};

UCLASS()
class UVbAbilitySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeFromWidgetData(const FVbAbilityWidgetData& InAbilityWidgetData);
    void InitInput();
    void ClearSlot();

protected:
    virtual void NativeConstruct() override;

private:
    void CreateToolTipWidget();

    void OnAbilityActivated(UGameplayAbility* ActivatedAbility);
    void OnStartCooldown(FGameplayTag Tag, int32 NewCount);
    void UpdateCooldown();
    void StopCooldown();

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="ToolTip")
    TSubclassOf<UVbAbilityToolTip> AbilityToolTipClass;

    UPROPERTY(EditDefaultsOnly, Category="ToolTip")
    float CooldownUpdateInterval = 0.05f;

    UPROPERTY(EditDefaultsOnly, Category="ToolTip")
    FName CooldownMatParam = "CooldownPercent";

    // ==== Runtime ==== //
    FNumberFormattingOptions CooldownFormattingOptions;
    FTimerHandle CooldownTimerHandle;

    UPROPERTY()
    FVbAbilityWidgetData AbilityWidgetData;

    UPROPERTY()
    FGameplayTag CooldownTag;

    UPROPERTY()
    UAbilitySystemComponent* ASC;

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UImage* Icon;

    UPROPERTY(meta=(BindWidget))
    UImage* CooldownIcon;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CooldownText;

    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* InputWidget;

    UPROPERTY(meta=(BindWidgetAnim), Transient)
    UWidgetAnimation* PulseAnimation;
};