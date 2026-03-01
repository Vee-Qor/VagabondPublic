// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Player/VbPlayerController.h"

#include "Character/VbCharacter.h"
#include "Data/VbCharacterData.h"
#include "UI/HUD/VbGameplayHUD.h"

void AVbPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    const AVbCharacter* OwnerCharacter = Cast<AVbCharacter>(InPawn);
    if (!OwnerCharacter) return;

    const UVbCharacterData* CharacterData = OwnerCharacter->GetCharacterData();
    if (!CharacterData) return;

    SetGenericTeamId(FGenericTeamId(static_cast<uint8>(CharacterData->GetDefaultTeamID())));
}

void AVbPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameOnly());
}

void AVbPlayerController::Client_InitOnScreenOverlay_Implementation(UAbilitySystemComponent* EnemyASC)
{
    if (const AVbGameplayHUD* GameplayHUD = Cast<AVbGameplayHUD>(GetHUD()))
    {
        GameplayHUD->InitializeOnScreenOverlay(EnemyASC);
    }
}

void AVbPlayerController::Client_DisableOnScreenOverlay_Implementation()
{
    if (const AVbGameplayHUD* GameplayHUD = Cast<AVbGameplayHUD>(GetHUD()))
    {
        GameplayHUD->DisableOnScreenOverlay();
    }
}