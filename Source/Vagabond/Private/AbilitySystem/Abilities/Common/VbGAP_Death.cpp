// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGAP_Death.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/VbCharacterData.h"
#include "Framework/VbGameMode.h"
#include "VbGameplayTags.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

UVbGAP_Death::UVbGAP_Death()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_System_Death.GetTag().GetSingleTagContainer());
    ActivationOwnedTags.AddTag(VbGameplayTags::State_Dying);
    CancelAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    ActivationBlockedTags.RemoveTag(VbGameplayTags::Effect_Stun);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag = VbGameplayTags::Event_Ability_Death;
    AbilityTriggers.Add(TriggerData);
}

void UVbGAP_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    DeathReward(TriggerEventData);

    UAbilityTask_PlayMontageAndWait* PlayDeathMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,
        GetDeathMontage(TriggerEventData), 1.0f, NAME_None, false);
    PlayDeathMontage->ReadyForActivation();

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitRagdollEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Ragdoll);
        WaitRagdollEvent->EventReceived.AddDynamic(this, &UVbGAP_Death::ApplyDeathStatus);
        WaitRagdollEvent->ReadyForActivation();
    }
}

UAnimMontage* UVbGAP_Death::GetDeathMontage(const FGameplayEventData* TriggerEventData) const
{
    if (!TriggerEventData) return nullptr;

    const UVbCharacterData* CharacterData = GetCharacterData();
    if (!CharacterData) return nullptr;

    const TArray<UAnimMontage*>& DeathMontages = CharacterData->GetDeathMontages();
    if (DeathMontages.IsEmpty()) return nullptr;

    return DeathMontages[TriggerEventData->EventMagnitude];
}

void UVbGAP_Death::ApplyDeathStatus(FGameplayEventData EventData)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;

    const AVbGameMode* GameMode = Cast<AVbGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GameMode) return;

    const TSubclassOf<UGameplayEffect> DeadEffectClass = GameMode->GetDeadEffectClass();
    if (!DeadEffectClass) return;

    const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DeadEffectClass, 1.0f, ContextHandle);

    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    K2_EndAbility();
}

void UVbGAP_Death::DeathReward(const FGameplayEventData* TriggerEventData)
{
    if (!TriggerEventData) return;

    AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
    if (!Killer || !Killer->GetInstigatorController() || !Killer->GetInstigatorController()->IsPlayerController())
    {
        Killer = nullptr;
    }

    TArray<AActor*> RewardTargets = GetRewardTargets();
    if (RewardTargets.IsEmpty() && !Killer) return;

    if (Killer && !RewardTargets.Contains(Killer))
    {
        RewardTargets.Add(Killer);
    }

    const UAbilitySystemComponent* AvatarASC = GetAbilitySystemComponentFromActorInfo();
    if (!AvatarASC) return;

    bool bFound = false;
    const float SelfExperience = AvatarASC->GetGameplayAttributeValue(UVbAttributeSet::GetExperienceAttribute(), bFound);

    const float TotalExperienceReward = ExperienceReward + ExperienceRewardPerExperience * SelfExperience;

    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffectClass);
    SpecHandle.Data->SetSetByCallerMagnitude(VbGameplayTags::Data_Experience, TotalExperienceReward);

    K2_ApplyGameplayEffectSpecToTarget(SpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
}

TArray<AActor*> UVbGAP_Death::GetRewardTargets() const
{
    TSet<AActor*> OutActors;

    const UWorld* World = GetWorld();
    if (!World) return OutActors.Array();

    const AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!GetWorld() || !Avatar) return OutActors.Array();

    const IGenericTeamAgentInterface* OwnerActorTeamAgent = Cast<IGenericTeamAgentInterface>(Avatar->GetInstigatorController());
    if (!OwnerActorTeamAgent) return OutActors.Array();

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(RangeReward);

    TArray<FOverlapResult> OverlapResults;
    GetWorld()->OverlapMultiByObjectType(OverlapResults, Avatar->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape);
    for (const FOverlapResult& OverlapResult : OverlapResults)
    {
        const AActor* OverlapActor = OverlapResult.GetActor();
        if (!OverlapActor) continue;

        const AController* Controller = OverlapActor->GetInstigatorController();
        if (!Controller) continue;

        const IGenericTeamAgentInterface* OverlapActorTeamAgent = Cast<IGenericTeamAgentInterface>(Controller);
        if (!OverlapActorTeamAgent || OwnerActorTeamAgent->GetGenericTeamId() == OverlapActorTeamAgent->GetGenericTeamId()) continue;

        if (!Controller->IsPlayerController()) continue;

        OutActors.Add(OverlapResult.GetActor());
    }

    return OutActors.Array();
}