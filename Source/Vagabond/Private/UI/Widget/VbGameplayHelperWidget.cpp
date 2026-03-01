// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbGameplayHelperWidget.h"

#include "VbCoreTypes.h"
#include "Character/VbInteractionComponent.h"
#include "Framework/VbFunctionLibrary.h"
#include "VbGameplayTags.h"
#include "VbInputWidget.h"
#include "Character/VbCombatComponent.h"
#include "Weapon/VbWeapon.h"

void UVbGameplayHelperWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetupAllWidgets();

    const APawn* OwnerPawn = GetOwningPlayerPawn();
    if (!OwnerPawn) return;

    UVbInteractionComponent* InteractionComponent = OwnerPawn->FindComponentByClass<UVbInteractionComponent>();
    if (!InteractionComponent) return;

    InteractionComponent->OnInteractionTargetChanged.AddUObject(this, &UVbGameplayHelperWidget::InteractionTargetChanged);
    InteractionTargetChanged(false);

    UVbCombatComponent* CombatComponent = OwnerPawn->FindComponentByClass<UVbCombatComponent>();
    if (!CombatComponent) return;

    CombatComponent->OnCurrentWeaponChanged.AddUObject(this, &UVbGameplayHelperWidget::OnCurrentWeaponChanged);
    OnCurrentWeaponChanged(nullptr);
}

void UVbGameplayHelperWidget::SetupAllWidgets()
{
    const UDataTable* KeyIconTable = UVbFunctionLibrary::GetKeyIconRow(GetWorld());
    if (!KeyIconTable) return;

    SetupInputWidget(InteractionInputWidget, VbGameplayTags::Input_Interaction, KeyIconTable, InteractionText);
    SetupInputWidget(InventoryInputWidget, VbGameplayTags::Input_Inventory_Open, KeyIconTable, InventoryText);
    SetupInputWidget(EquipmentInputWidget, VbGameplayTags::Input_Equipment_Open, KeyIconTable, EquipmentText);
    SetupInputWidget(EquipWeaponInputWidget, VbGameplayTags::Input_Weapon_Equip, KeyIconTable, EquipWeaponText);
    SetupInputWidget(SwitchWeaponInputWidget, VbGameplayTags::Input_Weapon_Switch, KeyIconTable, SwitchWeaponText);
}

void UVbGameplayHelperWidget::SetupInputWidget(UVbInputWidget* Widget, const FGameplayTag& InputTag, const UDataTable* KeyIconTable, const FText& Text)
{
    if (!Widget || !KeyIconTable) return;

    const FKey* KeyPtr = UVbFunctionLibrary::GetKeyForInputTag(GetOwningPlayerPawn(), InputTag);
    if (!KeyPtr) return;

    for (const auto& KetIconPair : KeyIconTable->GetRowMap())
    {
        const FVbKeyIconRow* Row = KeyIconTable->FindRow<FVbKeyIconRow>(KetIconPair.Key, "");
        if (Row->Key != *KeyPtr) continue;

        Widget->SetInput(Row->Texture.LoadSynchronous(), Text);
        Widget->SetVisibility(ESlateVisibility::Visible);
        return;
    }

    Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void UVbGameplayHelperWidget::InteractionTargetChanged(bool bHave)
{
    if (InteractionInputWidget)
    {
        InteractionInputWidget->SetVisibility(bHave ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UVbGameplayHelperWidget::OnCurrentWeaponChanged(const AVbWeapon* Weapon)
{
    if (!EquipWeaponInputWidget || !SwitchWeaponInputWidget) return;

    const ESlateVisibility NewVisibility = Weapon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

    EquipWeaponInputWidget->SetVisibility(NewVisibility);
    SwitchWeaponInputWidget->SetVisibility(NewVisibility);
}