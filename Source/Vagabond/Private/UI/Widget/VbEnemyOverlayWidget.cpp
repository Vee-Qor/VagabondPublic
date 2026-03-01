// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbEnemyOverlayWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "Character/VbCharacter.h"
#include "Components/TextBlock.h"
#include "Data/VbCharacterData.h"
#include "GameFramework/PlayerController.h"
#include "VbValueGaugeWidget.h"

void UVbEnemyOverlayWidget::InitializeOverlay(UAbilitySystemComponent* ASC, EVbOverlayMode InMode)
{
    if (ASC)
    {
        OverlayMode = InMode;
        BindToASC(ASC);
        SetActive(OverlayMode == EVbOverlayMode::OnScreen);
    }
}

void UVbEnemyOverlayWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
    if (ChangeData.NewValue <= 0.0f)
    {
        if (OverlayMode == EVbOverlayMode::OnScreen)
        {
            HideOverlay();
        }
        else
        {
            SetActive(false);
            StartHideTimer(HideDelayAfterDeath);
        }

        return;
    }

    if (OverlayMode == EVbOverlayMode::OverHead && ChangeData.NewValue < ChangeData.OldValue)
    {
        SetActive(true);
        StartHideTimer(HideDelayAfterHit);
    }
}

void UVbEnemyOverlayWidget::StartHideTimer(float HideDelay)
{
    if (OverlayMode != EVbOverlayMode::OverHead) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(HideTimerHandle);
    World->GetTimerManager().SetTimer(HideTimerHandle, FTimerDelegate::CreateUObject(this, &UVbEnemyOverlayWidget::SetActive, false), HideDelay,
        false);
}

void UVbEnemyOverlayWidget::UpdateOverlaySize()
{
    const UWorld* World = GetWorld();
    if (!World || !HealthGaugeWidget) return;

    const AActor* Target = OwnerActor.Get();
    if (!Target) return;

    const APlayerController* OwningPlayer = GetOwningPlayer();
    if (!OwningPlayer || !OwningPlayer->IsLocalController()) return;

    FVector CameraLocation;
    FRotator CameraRotation;
    OwningPlayer->GetPlayerViewPoint(CameraLocation, CameraRotation);

    const FVector TargetLocation = Target->GetActorLocation();

    const float Depth = FVector::DotProduct(TargetLocation - CameraLocation, CameraRotation.Vector());
    const float Alpha = FMath::Clamp((Depth - MinDepth) / (MaxDepth - MinDepth), 0.0f, 1.0f);
    const float TargetScale = FMath::Lerp(NearScale, FarScale, Alpha);

    CurrentScale = FMath::FInterpTo(CurrentScale, TargetScale, World->GetDeltaSeconds(), ScaleInterpSpeed);

    FWidgetTransform XForm = GetRenderTransform();
    XForm.Scale = FVector2D(CurrentScale, CurrentScale);
    SetRenderTransform(XForm);
}

void UVbEnemyOverlayWidget::BindToASC(UAbilitySystemComponent* ASC)
{
    if (!ASC || !HealthGaugeWidget) return;

    UnbindFromASC();

    OwnerASC = ASC;
    OwnerActor = ASC->GetOwnerActor();

    HealthGaugeWidget->InitializeWithAttribute(ASC, UVbAttributeSet::GetHealthAttribute(), UVbAttributeSet::GetMaxHealthAttribute());

    if (NameText)
    {
        NameText->SetText(GetCharacterName(OwnerActor.Get()));
    }

    HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetHealthAttribute()).AddUObject(this,
        &UVbEnemyOverlayWidget::OnHealthChanged);
}

void UVbEnemyOverlayWidget::UnbindFromASC()
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(HideTimerHandle);
        World->GetTimerManager().ClearTimer(UpdateOverlayTimerHandle);
    }

    if (OwnerASC && HealthChangedHandle.IsValid())
    {
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
    }

    HealthChangedHandle.Reset();
    OwnerASC = nullptr;
    OwnerActor = nullptr;
}

void UVbEnemyOverlayWidget::SetActive(bool bActive)
{
    SetVisibility(bActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

    if (const UWorld* World = GetWorld())
    {
        if (!bActive)
        {
            World->GetTimerManager().ClearTimer(UpdateOverlayTimerHandle);
        }

        if (bActive && OverlayMode == EVbOverlayMode::OverHead)
        {
            World->GetTimerManager().SetTimer(UpdateOverlayTimerHandle, this, &UVbEnemyOverlayWidget::UpdateOverlaySize, UpdateSizeInterval, true);
        }
    }
}

FText UVbEnemyOverlayWidget::GetCharacterName(AActor* Actor) const
{
    if (const AVbCharacter* Character = Cast<AVbCharacter>(Actor))
    {
        if (Character->GetCharacterData())
        {
            return Character->GetCharacterData()->GetCharacterName();
        }
    }

    return FText::FromString("N/A");
}


void UVbEnemyOverlayWidget::HideOverlay()
{
    UnbindFromASC();
    SetActive(false);
}

void UVbEnemyOverlayWidget::NativeDestruct()
{
    UnbindFromASC();
    Super::NativeDestruct();
}