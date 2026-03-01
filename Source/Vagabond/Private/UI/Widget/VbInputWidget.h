// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbInputWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class UVbInputWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetInput(UTexture2D* Texture, const FText& Text);

private:
    UPROPERTY(meta=(BindWidget))
    UTextBlock* InputText;

    UPROPERTY(meta=(BindWidget))
    UImage* InputIcon;
};