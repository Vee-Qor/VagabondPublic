// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Player/VbPlayerState.h"

#include "AbilitySystem/VbAttributeSet.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AVbPlayerState::AVbPlayerState()
{
    SetNetUpdateFrequency(100.0f);
    SetMinNetUpdateFrequency(66.0f);

    VbAbilitySystemComponent = CreateDefaultSubobject<UVbAbilitySystemComponent>("VbAbilitySystemComponent");
    VbAbilitySystemComponent->SetIsReplicated(true);
    VbAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    VbAttributeSet = CreateDefaultSubobject<UVbAttributeSet>("VbAttributeSet");
}

void AVbPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbPlayerState, bReady);
}

UAbilitySystemComponent* AVbPlayerState::GetAbilitySystemComponent() const
{
    return GetVbAbilitySystemComponent();
}

void AVbPlayerState::SetReady(bool bNewReady)
{
    if (!HasAuthority()) return;

    if (bReady == bNewReady) return;

    bReady = bNewReady;
    OnReadyChanged.Broadcast(bReady);
}

void AVbPlayerState::OnRep_ExtractionZone() const
{
    if (ExtractionZone == nullptr) return;
}

void AVbPlayerState::Server_ToggleReady_Implementation()
{
    SetReady(!bReady);
}

void AVbPlayerState::OnRep_PlayerName()
{
    Super::OnRep_PlayerName();

    OnNameChanged.Broadcast(GetPlayerName());
}

void AVbPlayerState::OnRep_Ready() const
{
    OnReadyChanged.Broadcast(bReady);
}