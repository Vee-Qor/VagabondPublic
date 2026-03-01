// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbLobbyWidget.generated.h"

class USizeBox;
class UButton;
class UImage;
class APlayerState;
class UTextBlock;
class UVbTeamSlotWidget;
class UUniformGridPanel;

UCLASS()
class UVbLobbyWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    void UpdateTeamSlots();
    void OnPlayerStateReady(APlayerState* InPlayerState);
    void OnSessionStartTimer(bool bIsActive);
    void UpdateSessionRunText();
    void CreateTeamSlotWidget(USizeBox* SizeBox, APlayerState* PlayerState) const;
    void SetLeaveVisibility(bool bShow);

    UFUNCTION()
    void OnReadyClicked();

    UFUNCTION()
    void OnLeaveClicked();

    UFUNCTION()
    void OnReadyChanged(bool bReady);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UVbTeamSlotWidget> TeamSlotWidgetClass;

    UPROPERTY(EditDefaultsOnly)
    FLinearColor ReadyColor = FLinearColor::Red;

    UPROPERTY(EditDefaultsOnly)
    FLinearColor NotReadyColor = FLinearColor::Gray;

    UPROPERTY(EditDefaultsOnly)
    float StartDelayUpdateInterval = 0.2f;

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    USizeBox* FirstSizeBox;

    UPROPERTY(meta=(BindWidget))
    USizeBox* SecondSizeBox;

    UPROPERTY(meta=(BindWidget))
    USizeBox* ThirdSizeBox;

    UPROPERTY(meta=(BindWidget))
    UButton* ReadyBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* ReadyText;

    UPROPERTY(meta=(BindWidget))
    UButton* LeaveBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* LeaveText;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* StartDelayText;

    FNumberFormattingOptions NumberFormattingOptions;

    // ==== Runtime ==== //
    FTimerHandle MatchCountdownTimerHandle;
};