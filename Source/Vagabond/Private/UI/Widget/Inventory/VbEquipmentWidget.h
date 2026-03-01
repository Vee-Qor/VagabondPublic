// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbEquipmentWidget.generated.h"

class UVbEquipmentComponent;
class UUniformGridPanel;

UCLASS()
class UVbEquipmentWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    void ToggleEquipment();

private:
    UPROPERTY(meta=(BindWidget))
    UUniformGridPanel* EquipmentPanel;

    UPROPERTY()
    UVbEquipmentComponent* EquipmentComponent;

    void OnEquipmentChanged() const;
};