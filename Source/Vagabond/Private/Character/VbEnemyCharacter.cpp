// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbEnemyCharacter.h"

#include "AI/VbAIController.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/VbCharacterData.h"
#include "Data/Enemy/VbEnemyData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/VbEnemyOverlayWidget.h"
#include "UI/Widget/VbPlayerHUDWidget.h"

AVbEnemyCharacter::AVbEnemyCharacter()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AVbAIController::StaticClass();

    GetCharacterMovement()->MaxWalkSpeed = 400.0f;

    SetVbAbilitySystemComponent(CreateDefaultSubobject<UVbAbilitySystemComponent>("VbAbilitySystemComponent"));
    SetVbAttributeSet(CreateDefaultSubobject<UVbAttributeSet>("VbAttributeSet"));

    EnemyOverlayWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyOverlayWidgetComponent");
    EnemyOverlayWidgetComponent->SetupAttachment(GetRootComponent());
    EnemyOverlayWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EnemyOverlayWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    EnemyOverlayWidgetComponent->SetDrawAtDesiredSize(true);
}

void AVbEnemyCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    const UVbEnemyData* EnemyData = Cast<UVbEnemyData>(GetCharacterData());
    if (!EnemyData) return;

    const TSubclassOf<AVbAIController> NewAIControllerClass = EnemyData->GetAIControllerClass();
    if (!NewAIControllerClass) return;

    AIControllerClass = NewAIControllerClass;

    if (HasAuthority() && GetController() && GetController()->GetClass() != NewAIControllerClass)
    {
        if (AAIController* OldController = Cast<AAIController>(GetController()))
        {
            OldController->Destroy();
            SpawnDefaultController();
        }
    }
}

void AVbEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbEnemyCharacter, TargetActor);
}

void AVbEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializeAbilityActorInfo();

    if (const UVbEnemyData* EnemyData = Cast<UVbEnemyData>(GetCharacterData()))
    {
        if (EnemyData->GetEnemyType() == EVbEnemyType::Boss) return;

        InitializeOverlay();
    }
}

void AVbEnemyCharacter::InitializeAbilityActorInfo()
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        ASC->InitAbilityActorInfo(this, this);
    }

    Super::InitializeAbilityActorInfo();
}

void AVbEnemyCharacter::OnDying(const FGameplayTag Tag, int32 NewCount)
{
    Super::OnDying(Tag, NewCount);

    if (NewCount > 0)
    {
        const AVbAIController* AIController = Cast<AVbAIController>(GetController());
        if (!AIController) return;

        if (AIController->GetBrainComponent())
        {
            AIController->GetBrainComponent()->StopLogic("Dead");
        }

        AIController->SetAllSenseEnabled(false);
    }
}

void AVbEnemyCharacter::OnStun(const FGameplayTag Tag, int32 NewCount)
{
    Super::OnStun(Tag, NewCount);

    const AVbAIController* AIController = Cast<AVbAIController>(GetController());
    if (!AIController || !AIController->GetBrainComponent()) return;

    if (NewCount > 0)
    {
        AIController->GetBrainComponent()->StopLogic("Stun");
    }
    else
    {
        AIController->GetBrainComponent()->StartLogic();
    }
}

void AVbEnemyCharacter::InitializeOverlay() const
{
    if (const UVbEnemyData* EnemyData = Cast<UVbEnemyData>(GetCharacterData()))
    {
        EnemyOverlayWidgetComponent->SetWidgetClass(EnemyData->GetEnemyOverlayWidgetClass());

        if (UVbEnemyOverlayWidget* EnemyOverlayWidget = Cast<UVbEnemyOverlayWidget>(EnemyOverlayWidgetComponent->GetUserWidgetObject()))
        {
            EnemyOverlayWidget->InitializeOverlay(GetAbilitySystemComponent(), EVbOverlayMode::OverHead);
        }
    }
}