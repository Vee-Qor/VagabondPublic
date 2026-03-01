// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbUserWidget.generated.h"

class UVbInventoryComponent;
class UVbEquipmentComponent;

UCLASS()
class UVbUserWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UVbEquipmentComponent* GetEquipmentComponent() const;
    UVbInventoryComponent* GetInventoryComponent() const;

    void SetShowMouseCursor(bool bShow) const;
    void SetInputEnabled(bool bEnable) const;
    void SetFocusToGameAndUI() const;
    void SetFocusToGameOnly() const;

private:
    UPROPERTY()
    APlayerController* PlayerController;
};