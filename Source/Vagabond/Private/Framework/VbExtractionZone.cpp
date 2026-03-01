// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbExtractionZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "VbGameState.h"
#include "VbGameplayTags.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

AVbExtractionZone::AVbExtractionZone()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxOverlap = CreateDefaultSubobject<UBoxComponent>("BoxOverlap");
    SetRootComponent(BoxOverlap);
}

void AVbExtractionZone::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        BoxOverlap->OnComponentBeginOverlap.AddDynamic(this, &AVbExtractionZone::OnBoxBeginOverlap);
        BoxOverlap->OnComponentEndOverlap.AddDynamic(this, &AVbExtractionZone::OnBoxEndOverlap);
    }
}

bool AVbExtractionZone::IsPlayer(AActor* Actor) const
{
    if (!Actor) return false;

    const AController* Controller = Actor->GetInstigatorController();
    if (!Controller) return false;

    return Controller ? Controller->IsPlayerController() : false;
}

bool AVbExtractionZone::IsExtractionReady() const
{
    const UWorld* World = GetWorld();
    if (!World) return false;

    const AGameStateBase* GameStateBase = World->GetGameState();
    if (!GameStateBase) return false;

    for (const APlayerState* PlayerState : GameStateBase->PlayerArray)
    {
        if (!PlayerState) return false;

        const UAbilitySystemComponent* PlayerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerState->GetPawn());
        if (!PlayerASC) return false;
        if (PlayerASC->HasMatchingGameplayTag(VbGameplayTags::State_Dead) || PlayerASC->HasMatchingGameplayTag(VbGameplayTags::State_Dying)) return false;
        if (!PlayersInZone.Contains(PlayerState)) return false;
    }

    return true;
}

void AVbExtractionZone::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !IsPlayer(OtherActor)) return;

    const AController* Controller = OtherActor->GetInstigatorController();
    if (!Controller || !Controller->IsPlayerController()) return;

    APlayerState* PlayerState = Controller->PlayerState;
    if (!PlayerState) return;

    PlayersInZone.Add(PlayerState);

    if (!IsExtractionReady()) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    AVbGameState* GameState = World->GetGameState<AVbGameState>();
    if (!GameState) return;

    GameState->StartExtraction();
}

void AVbExtractionZone::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor || !IsPlayer(OtherActor)) return;

    const AController* Controller = OtherActor->GetInstigatorController();
    if (!Controller || !Controller->IsPlayerController()) return;

    const APlayerState* PlayerState = Controller->PlayerState;
    if (!PlayerState) return;

    PlayersInZone.Remove(PlayerState);

    const UWorld* World = GetWorld();
    if (!World) return;

    AVbGameState* GameState = World->GetGameState<AVbGameState>();
    if (!GameState) return;

    GameState->StopExtraction();
}