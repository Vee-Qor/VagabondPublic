// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/VbUserWidget.h"
#include "VbEnemyOverlayWidget.generated.h"

class UAbilitySystemComponent;
class UVbValueGaugeWidget;
class UTextBlock;
struct FOnAttributeChangeData;

UENUM()
enum class EVbOverlayMode : uint8 { OverHead, OnScreen };

UCLASS()
class UVbEnemyOverlayWidget : public UVbUserWidget
{
    GENERATED_BODY()

public:
    void InitializeOverlay(UAbilitySystemComponent* ASC, EVbOverlayMode InMode);
    void HideOverlay();

protected:
    virtual void NativeDestruct() override;

private:
    void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
    void StartHideTimer(float HideDelay);
    void UpdateOverlaySize();
    void BindToASC(UAbilitySystemComponent* ASC);
    void UnbindFromASC();
    void SetActive(bool bActive);
    
    FText GetCharacterName(AActor* Actor) const;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1.0f))
    float MinDepth = 300.0f;

    UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1.0f))
    float MaxDepth = 2500.0f;

    UPROPERTY(EditDefaultsOnly)
    float NearScale = 1.0f;

    UPROPERTY(EditDefaultsOnly)
    float FarScale = 0.65f;

    UPROPERTY(EditDefaultsOnly)
    float ScaleInterpSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly)
    float UpdateSizeInterval = 0.1f;

    UPROPERTY(EditDefaultsOnly)
    float HideDelayAfterHit = 30.0f;

    UPROPERTY(EditDefaultsOnly)
    float HideDelayAfterDeath = 2.5f;

    UPROPERTY(meta=(BindWidget))
    UVbValueGaugeWidget* HealthGaugeWidget;

    UPROPERTY(meta=(BindWidget))
    UTextBlock* NameText;

    // ==== Runtime ==== //
    UPROPERTY()
    EVbOverlayMode OverlayMode = EVbOverlayMode::OverHead;

    UPROPERTY()
    FTimerHandle HideTimerHandle;

    UPROPERTY()
    FTimerHandle UpdateOverlayTimerHandle;

    UPROPERTY()
    TWeakObjectPtr<AActor> OwnerActor;

    UPROPERTY()
    UAbilitySystemComponent* OwnerASC;

    FDelegateHandle HealthChangedHandle;
    float CurrentScale = 1.0f;
};