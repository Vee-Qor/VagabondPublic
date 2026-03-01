// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbValueGaugeWidget.generated.h"

class USizeBox;
class UAbilitySystemComponent;
class UTextBlock;
class UImage;
struct FGameplayAttribute;
struct FOnAttributeChangeData;

UCLASS()
class UVbValueGaugeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeWithAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeDestruct() override;

private:
    void AttributeChanged(const FOnAttributeChangeData& ChangeData);
    void MaxAttributeChanged(const FOnAttributeChangeData& ChangeData);
    void UpdateDisplay();

    void StartDelayedTimer();
    void StopDelayedTimer();
    void UpdateDelayedPercent();

    // ==== Config ==== //
    UPROPERTY(EditAnywhere, Category="Size")
    float BoxSizeX = 400.0f;

    UPROPERTY(EditAnywhere, Category="Size")
    float BoxSizeY = 30.0f;
    
    UPROPERTY(EditAnywhere, Category="Size")
    int32 FontSize = 18;

    UPROPERTY(EditAnywhere, Category="Color")
    FLinearColor MainColor = FLinearColor(0.175f, 0.0f, 0.0f, 1.0f);

    UPROPERTY(EditAnywhere, Category="Color")
    FLinearColor DelayedColor = FLinearColor(0.5f, 0.1f, 0.05f, 1.0f);

    UPROPERTY(EditAnywhere, Category="Color")
    FLinearColor BackgroundColor = FLinearColor(0.05f, 0.05f, 0.05f, 0.0f);

    UPROPERTY(EditAnywhere, Category="Update")
    float DelayedInterpSpeed = 5.0f;

    UPROPERTY(EditAnywhere, Category="Update")
    float DelayedUpdateInterval = 0.05f;

    UPROPERTY(EditAnywhere, Category="ParamName")
    FName MainColorMatParamName = "MainColor";

    UPROPERTY(EditAnywhere, Category="ParamName")
    FName DelayedColorMatParamName = "DelayedColor";

    UPROPERTY(EditAnywhere, Category="ParamName")
    FName BackgroundColorMatParamName = "BackgroundColor";

    UPROPERTY(EditAnywhere, Category="ParamName")
    FName MainPercentMatParamName = "MainPercent";

    UPROPERTY(EditAnywhere, Category="ParamName")
    FName DelayedPercentMatParamName = "DelayedPercent";

    UPROPERTY(meta=(BindWidget))
    USizeBox* SizeBox;

    UPROPERTY(meta=(BindWidget))
    UImage* ValueBar;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* ValueText;

    FNumberFormattingOptions NumberFormattingOptions;

    // ==== Runtime ==== //
    UPROPERTY()
    UMaterialInstanceDynamic* BarMaterial;

    FTimerHandle DelayedUpdateTimerHandle;
    float MainPercent = 0.0f;
    float DelayedPercent = 0.0f;

    float CachedValue = 0.0f;
    float CachedMaxValue = 0.0f;
};