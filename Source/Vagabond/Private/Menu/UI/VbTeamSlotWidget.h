// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VbTeamSlotWidget.generated.h"

class UBorder;
class UButton;
class UTextBlock;
class UImage;
class AVbPlayerState;

UCLASS()
class UVbTeamSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitSlot(APlayerState* InPlayerState);
    void ClearSlot();

protected:
    virtual void NativeConstruct() override;

private:
    void SetSteamAvatar() const;
    void UnbindFromPlayerState();

    UFUNCTION()
    void OnSlotClicked();

    UFUNCTION()
    void OnNameChanged(const FString& Name);

    UFUNCTION()
    void OnReadyChanged(bool bReady);

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, Category="Color")
    UTexture2D* DefaultIcon;

    UPROPERTY(EditDefaultsOnly, Category="Color")
    FLinearColor ReadyColor = FLinearColor::Red;

    UPROPERTY(EditDefaultsOnly, Category="Color")
    FLinearColor NotReadyColor = FLinearColor::Gray;

    // ==== BindWidget ==== //
    UPROPERTY(meta=(BindWidget))
    UImage* AvatarIcon;

    UPROPERTY(meta=(BindWidget))
    UButton* SlotBtn;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* NameText;

    // ==== Runtime ==== //
    UPROPERTY()
    AVbPlayerState* PlayerState;
};