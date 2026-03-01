// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbLoadingScreenWidget.generated.h"

class UCircularThrobber;
class UImage;
class UTextBlock;

UCLASS()
class UVbLoadingScreenWidget : public UUserWidget
{
    GENERATED_BODY()

private:
    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UTextBlock* LoadingText;

    UPROPERTY(meta=(BindWidget))
    UCircularThrobber* Throbber;

    UPROPERTY(meta=(BindWidget))
    UImage* BackgroundIcon;
};