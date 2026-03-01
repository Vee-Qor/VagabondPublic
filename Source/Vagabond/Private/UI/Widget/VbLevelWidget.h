// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbLevelWidget.generated.h"

class UAbilitySystemComponent;
class UTextBlock;
class UImage;
struct FOnAttributeChangeData;

UCLASS()
class UVbLevelWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void UpdateLevel(const FOnAttributeChangeData& ChangeData);

    UPROPERTY(EditDefaultsOnly)
    FName PercentMatParamName = "Percent";

    UPROPERTY()
    UAbilitySystemComponent* OwnerASC;

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UImage* LevelProgressIcon;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* LevelText;
};