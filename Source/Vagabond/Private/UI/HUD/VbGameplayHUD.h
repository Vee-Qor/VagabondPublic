// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VbGameplayHUD.generated.h"

class UAbilitySystemComponent;
class UVbAbilitySystemComponent;
class UVbEquipmentWidget;
class UVbEnemyOverlayWidget;
class UVbGameMenuWidget;
class UVbInventoryWidget;
class UVbPlayerHUDWidget;

UCLASS()
class AVbGameplayHUD : public AHUD
{
    GENERATED_BODY()

public:
    void CreateHUDAndInitializeWithASC(UVbAbilitySystemComponent* ASC);

    void ToggleInventory() const;
    void ToggleEquipment() const;
    void ToggleGameMenu() const;
    void InitializeOnScreenOverlay(UAbilitySystemComponent* ASC) const;
    void DisableOnScreenOverlay() const;

private:
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UVbPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY()
    UVbPlayerHUDWidget* PlayerHUDWidget;
};