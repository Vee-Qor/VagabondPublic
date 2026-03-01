// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbItemToolTipWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class UVbItemToolTipWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetInfo(UTexture2D* Texture, const FText& Name, const FText& Description);

private:
    UPROPERTY(meta=(BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* DescriptionText;

    UPROPERTY(meta=(BindWidget))
    UImage* Icon;
};