// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbAbilityToolTip.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class UVbAbilityToolTip : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetInfo(UTexture2D* Texture, const FText& Name, const FText& Description, float Damage, float Cooldown, float Cost);

private:
    UPROPERTY(meta=(BindWidget))
    UImage* Icon;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* DescriptionText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* DamageValueText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CooldownValueText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* CostValueText;
};