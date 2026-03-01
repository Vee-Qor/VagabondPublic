// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "UI/Widget/VbWinWidget.h"

#include "Components/TextBlock.h"
#include "Framework/VbGameState.h"

void UVbWinWidget::NativeConstruct()
{
    Super::NativeConstruct();

    NumberFormattingOptions.SetMaximumFractionalDigits(0);

    const UWorld* World = GetWorld();
    if (!World) return;

    GameState = World->GetGameState<AVbGameState>();
    if (!GameState) return;

    GameState->OnExtractionCountdownUpdated.AddUObject(this, &UVbWinWidget::OnExtractionStart);
}

void UVbWinWidget::OnExtractionStart(bool bIsActive)
{
    const UWorld* World = GetWorld();
    if (!World) return;

    if (bIsActive)
    {
        World->GetTimerManager().SetTimer(ExtractionTimer, this, &UVbWinWidget::UpdateExtractionText, 0.1f, true);
    }
    else
    {
        World->GetTimerManager().ClearTimer(ExtractionTimer);
    }

    SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UVbWinWidget::UpdateExtractionText() const
{
    const float CurrentServerTime = GameState->GetServerWorldTimeSeconds();
    const float EndServerTime = GameState->GetExtractionEndTimeServer();

    const float TimeRemaining = FMath::Max(0.0f, EndServerTime - CurrentServerTime);

    if (!CountdownText) return;

    CountdownText->SetText(FText::AsNumber(TimeRemaining, &NumberFormattingOptions));
}