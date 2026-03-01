// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbAbilityPanelWidget.generated.h"

class UVbAbilitySlotWidget;
class UVbAbilitySystemComponent;
class UGameplayAbility;
struct FVbAbilityWidgetData;
struct FGameplayTag;

UCLASS()
class UVbAbilityPanelWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void OnArmedTagChanged(FGameplayTag Tag, int32 NewCount);
    void UpdateSlots();
    void ClearSlots() const;
    const FVbAbilityWidgetData* GetAbilityWidgetData(const TSubclassOf<UGameplayAbility>& AbilityClass) const;

    FTimerHandle UpdateTimerHandle;

    UPROPERTY()
    UVbAbilitySystemComponent* ASC;

    UPROPERTY(EditDefaultsOnly)
    UDataTable* AbilityDataRow;

    UPROPERTY(meta=(BindWidget))
    UVbAbilitySlotWidget* FirstSlot;

    UPROPERTY(meta=(BindWidget))
    UVbAbilitySlotWidget* SecondSlot;

    UPROPERTY(meta=(BindWidget))
    UVbAbilitySlotWidget* ThirdSlot;

    UPROPERTY(meta=(BindWidget))
    UVbAbilitySlotWidget* FourthSlot;
};