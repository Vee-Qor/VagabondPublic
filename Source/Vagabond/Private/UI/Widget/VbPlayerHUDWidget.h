// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbPlayerHUDWidget.generated.h"

class UVbAbilityPanelWidget;
class UVbAbilitySystemComponent;
class UVbCrosshairWidget;
class UVbDeathWidget;
class UVbEquipmentWidget;
class UVbEnemyOverlayWidget;
class UVbGameMenuWidget;
class UVbGameplayHelperWidget;
class UVbInventoryWidget;
class UVbLevelWidget;
class UVbValueGaugeWidget;
class UVbWinWidget;

UCLASS()
class UVbPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    FORCEINLINE UVbValueGaugeWidget* GetHealthGaugeWidget() const { return HealthGaugeWidget; }
    FORCEINLINE UVbValueGaugeWidget* GetResourceGaugeWidget() const { return ResourceGaugeWidget; }
    FORCEINLINE UVbEnemyOverlayWidget* GetEnemyOverlayWidget() const { return EnemyOverlayWidget; }
    FORCEINLINE UVbInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }
    FORCEINLINE UVbEquipmentWidget* GetEquipmentWidget() const { return EquipmentWidget; }
    FORCEINLINE UVbGameMenuWidget* GetGameMenuWidget() const { return GameMenuWidget; }
    FORCEINLINE UVbDeathWidget* GetDeathWidget() const { return DeathWidget; }
    FORCEINLINE UVbWinWidget* GetWinWidget() const { return WinWidget; }

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta=(BindWidget))
    UVbValueGaugeWidget* HealthGaugeWidget;

    UPROPERTY(meta=(BindWidget))
    UVbValueGaugeWidget* ResourceGaugeWidget;

    UPROPERTY(meta=(BindWidget))
    UVbEnemyOverlayWidget* EnemyOverlayWidget;

    UPROPERTY(meta=(BindWidget))
    UVbInventoryWidget* InventoryWidget;

    UPROPERTY(meta=(BindWidget))
    UVbEquipmentWidget* EquipmentWidget;

    UPROPERTY(meta=(BindWidget))
    UVbCrosshairWidget* CrosshairWidget;

    UPROPERTY(meta=(BindWidget))
    UVbGameMenuWidget* GameMenuWidget;

    UPROPERTY(meta=(BindWidget))
    UVbDeathWidget* DeathWidget;

    UPROPERTY(meta=(BindWidget))
    UVbAbilityPanelWidget* AbilityPanelWidget;

    UPROPERTY(meta=(BindWidget))
    UVbGameplayHelperWidget* GameplayHelperWidget;

    UPROPERTY(meta=(BindWidget))
    UVbLevelWidget* LevelWidget;

    UPROPERTY(meta=(BindWidget))
    UVbWinWidget* WinWidget;
};