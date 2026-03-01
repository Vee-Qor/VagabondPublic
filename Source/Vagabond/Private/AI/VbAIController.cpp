// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AI/VbAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/VbEnemyCharacter.h"
#include "Data/VbCharacterData.h"
#include "Data/Enemy/VbEnemyData.h"
#include "VbGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "Player/VbPlayerController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

AVbAIController::AVbAIController()
{
    AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfig_Sight");
    AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
    AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
    AISenseConfig_Sight->SightRadius = 1500.0f;
    AISenseConfig_Sight->LoseSightRadius = 2000.0f;
    AISenseConfig_Sight->PeripheralVisionAngleDegrees = 90.0f;
    AISenseConfig_Sight->SetMaxAge(5.0f);

    AISenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("AISenseConfig_Hearing");
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectEnemies = true;
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectFriendlies = true;
    AISenseConfig_Hearing->DetectionByAffiliation.bDetectNeutrals = true;
    AISenseConfig_Hearing->HearingRange = 1250.0f;
    AISenseConfig_Hearing->SetMaxAge(2.5f);

    AISenseConfig_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>("AISenseConfig_Damage");
    AISenseConfig_Damage->SetMaxAge(5.0f);

    EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
    EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
    EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Hearing);
    EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Damage);
    EnemyPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
    EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AVbAIController::OnEnemyPerceptionUpdated);
    EnemyPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AVbAIController::OnEnemyPerceptionForgotten);
}

void AVbAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    const AVbEnemyCharacter* EnemyCharacter = Cast<AVbEnemyCharacter>(InPawn);
    if (!EnemyCharacter) return;

    const UVbEnemyData* EnemyData = Cast<UVbEnemyData>(EnemyCharacter->GetCharacterData());
    if (!EnemyData) return;

    RunBehaviorTree(EnemyData->GetBehaviorTree());

    if (GetBlackboardComponent())
    {
        GetBlackboardComponent()->SetValueAsFloat(MeleeAttackDistanceKeyName, EnemyData->GetMeleeAttackDistance());
        GetBlackboardComponent()->SetValueAsFloat(RangeAttackDistanceKeyName, EnemyData->GetRangeAttackDistance());
        GetBlackboardComponent()->SetValueAsFloat(StrafeRangeKeyName, EnemyData->GetStrafeRange());
        GetBlackboardComponent()->SetValueAsFloat(ChasingRangeKeyName, EnemyData->GetChasingRange());
    }

    SetGenericTeamId(FGenericTeamId(static_cast<uint8>(EnemyData->GetDefaultTeamID())));
}

ETeamAttitude::Type AVbAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const APawn* OtherPawn = Cast<APawn>(&Other);
    if (!OtherPawn) return ETeamAttitude::Neutral;

    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());

    if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() != GetGenericTeamId())
    {
        return ETeamAttitude::Hostile;
    }

    return ETeamAttitude::Neutral;
}

void AVbAIController::SetAllSenseEnabled(bool bEnable) const
{
    if (EnemyPerceptionComponent && AISenseConfig_Sight && AISenseConfig_Hearing && AISenseConfig_Damage)
    {
        EnemyPerceptionComponent->SetSenseEnabled(AISenseConfig_Sight->GetSenseImplementation(), bEnable);
        EnemyPerceptionComponent->SetSenseEnabled(AISenseConfig_Hearing->GetSenseImplementation(), bEnable);
        EnemyPerceptionComponent->SetSenseEnabled(AISenseConfig_Damage->GetSenseImplementation(), bEnable);
    }
}

void AVbAIController::OnEnemyPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
    const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        if (!TargetActor || IsTargetDead(TargetActor)) return;

        SetOverlayEnabled(true, TargetActor);

        if (BlackboardComponent->GetValueAsObject(TargetActorKeyName) == nullptr)
        {
            SetTarget(TargetActor);
        }
    }
    else
    {
        if (GetCurrentTarget() == TargetActor)
        {
            ClearTarget();
            SelectTarget();
        }
    }
}

void AVbAIController::OnEnemyPerceptionForgotten(AActor* ForgotActor)
{
    SetOverlayEnabled(false, ForgotActor);

    if (GetCurrentTarget() == ForgotActor)
    {
        ClearTarget();
    }
}

void AVbAIController::SetTargetToAim(AActor* TargetActor) const
{
    if (AVbEnemyCharacter* EnemyCharacter = Cast<AVbEnemyCharacter>(GetPawn()))
    {
        EnemyCharacter->SetTargetActor(TargetActor);
    }
}

void AVbAIController::SetOverlayEnabled(bool bEnabled, const AActor* TargetActor) const
{
    const AVbCharacter* OwnerCharacter = Cast<AVbCharacter>(GetPawn());
    if (!OwnerCharacter) return;

    const UVbEnemyData* EnemyData = Cast<UVbEnemyData>(OwnerCharacter->GetCharacterData());

    if (!EnemyData || EnemyData->GetEnemyType() != EVbEnemyType::Boss) return;

    AVbPlayerController* PlayerController = Cast<AVbPlayerController>(TargetActor->GetInstigatorController());
    if (!PlayerController) return;

    if (bEnabled)
    {
        PlayerController->Client_InitOnScreenOverlay(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
    }
    else
    {
        PlayerController->Client_DisableOnScreenOverlay();
    }
}

void AVbAIController::SelectTarget()
{
    if (!EnemyPerceptionComponent || !GetPawn()) return;

    TArray<AActor*> PerceivedHostileActors;
    EnemyPerceptionComponent->GetPerceivedHostileActors(PerceivedHostileActors);

    AActor* BestTarget = nullptr;
    float BestDistanceSquared = FLT_MAX;

    const FVector OwnerLocation = GetPawn()->GetActorLocation();

    for (AActor* TargetActor : PerceivedHostileActors)
    {
        if (!TargetActor || IsTargetDead(TargetActor)) continue;

        const float DistanceSquared = FVector::DistSquared(OwnerLocation, TargetActor->GetActorLocation());

        if (DistanceSquared < BestDistanceSquared)
        {
            BestDistanceSquared = DistanceSquared;
            BestTarget = TargetActor;
        }
    }

    if (BestTarget)
    {
        SetTarget(BestTarget);
    }
}

void AVbAIController::SetTarget(AActor* TargetActor)
{
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        BlackboardComponent->SetValueAsObject(TargetActorKeyName, TargetActor);
    }

    SetTargetToAim(TargetActor);
}

void AVbAIController::ClearTarget()
{
    if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        BlackboardComponent->ClearValue(TargetActorKeyName);
    }

    SetTargetToAim(nullptr);
}

AActor* AVbAIController::GetCurrentTarget() const
{
    if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
    {
        return Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
    }

    return nullptr;
}

bool AVbAIController::IsTargetDead(AActor* TargetActor) const
{
    if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        return TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Dying) || TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Dead);
    }

    return false;
}