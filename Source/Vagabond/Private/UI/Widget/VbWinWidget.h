// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbUserWidget.h"
#include "VbWinWidget.generated.h"

class AVbGameState;
class UTextBlock;

UCLASS()
class UVbWinWidget : public UVbUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void OnExtractionStart(bool bIsActive);
    void UpdateExtractionText() const;
    
    FNumberFormattingOptions NumberFormattingOptions;
    FTimerHandle ExtractionTimer;
    
    UPROPERTY()
    AVbGameState* GameState;
    
    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UTextBlock* CountdownText;
};