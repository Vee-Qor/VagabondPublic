// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbGameplayHelperWidget.generated.h"

class AVbWeapon;
class UVbInputWidget;
struct FGameplayTag;

UCLASS()
class UVbGameplayHelperWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void SetupAllWidgets();
    void SetupInputWidget(UVbInputWidget* Widget, const FGameplayTag& InputTag, const UDataTable* KeyIconTable, const FText& Text);

    void InteractionTargetChanged(bool bHave);
    void OnCurrentWeaponChanged(const AVbWeapon* Weapon);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly)
    FText InteractionText = INVTEXT("Use");

    UPROPERTY(EditDefaultsOnly)
    FText InventoryText = INVTEXT("Inventory");

    UPROPERTY(EditDefaultsOnly)
    FText EquipmentText = INVTEXT("Equipment");

    UPROPERTY(EditDefaultsOnly)
    FText EquipWeaponText = INVTEXT("Equip Weapon");

    UPROPERTY(EditDefaultsOnly)
    FText SwitchWeaponText = INVTEXT("Switch Weapon");

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* InteractionInputWidget;

    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* EquipmentInputWidget;

    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* InventoryInputWidget;

    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* EquipWeaponInputWidget;

    UPROPERTY(meta=(BindWidget))
    UVbInputWidget* SwitchWeaponInputWidget;
};