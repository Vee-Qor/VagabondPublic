// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_RainOfArrows.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/TargetActor/VbTargetActor_GroundArea.h"
#include "VbCoreTypes.h"
#include "Character/VbCombatComponent.h"
#include "VbGameplayTags.h"
#include "Data/VbStyleData.h"
#include "Framework/VbFunctionLibrary.h"
#include "Weapon/VbProjectile.h"

UVbGA_RainOfArrows::UVbGA_RainOfArrows()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_RainOfArrows.GetTag().GetSingleTagContainer());
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);

    SetInputTag(VbGameplayTags::Input_AbilityThree);
}

void UVbGA_RainOfArrows::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ensure(RainOfArrowsMontage);
    ensure(TargetActorClass);

    UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed,
        TargetActorClass);
    WaitTargetData->ValidData.AddDynamic(this, &UVbGA_RainOfArrows::TargetConfirmed);
    WaitTargetData->Cancelled.AddDynamic(this, &UVbGA_RainOfArrows::TargetCanceled);
    WaitTargetData->ReadyForActivation();

    AGameplayAbilityTargetActor* TargetActor;
    WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

    if (AVbTargetActor_GroundArea* GroundAreaActor = Cast<AVbTargetActor_GroundArea>(TargetActor))
    {
        GroundAreaActor->SetTargetAreaRadius(AreaTraceInfo.Radius);
    }

    WaitTargetData->FinishSpawningActor(this, TargetActor);
}

void UVbGA_RainOfArrows::TargetConfirmed(const FGameplayAbilityTargetDataHandle& Data)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    const auto* Loc = static_cast<const FGameplayAbilityTargetData_LocationInfo*>(Data.Get(0));
    if (!Loc)
    {
        K2_EndAbility();
        return;
    }

    TargetActorPoint = Loc->TargetLocation.GetTargetingTransform().GetLocation();

    UpdateMotionWarpingTarget();

    UAbilityTask_PlayMontageAndWait* PlayRainOfArrowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None,
        RainOfArrowsMontage);
    PlayRainOfArrowMontage->OnCompleted.AddDynamic(this, &UVbGA_RainOfArrows::K2_EndAbility);
    PlayRainOfArrowMontage->OnCancelled.AddDynamic(this, &UVbGA_RainOfArrows::K2_EndAbility);
    PlayRainOfArrowMontage->OnBlendOut.AddDynamic(this, &UVbGA_RainOfArrows::K2_EndAbility);
    PlayRainOfArrowMontage->OnInterrupted.AddDynamic(this, &UVbGA_RainOfArrows::K2_EndAbility);
    PlayRainOfArrowMontage->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitAttackEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_End);
    WaitAttackEndEvent->EventReceived.AddDynamic(this, &UVbGA_RainOfArrows::OnAttackEnd);
    WaitAttackEndEvent->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitSpawnProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
        VbGameplayTags::Event_Attack_ProjectileSpawn);
    WaitSpawnProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_RainOfArrows::OnSpawnProjectile);
    WaitSpawnProjectileEvent->ReadyForActivation();
}

void UVbGA_RainOfArrows::TargetCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
    K2_EndAbility();
}

void UVbGA_RainOfArrows::OnAttackEnd(FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UVbGA_RainOfArrows::OnSpawnProjectile(FGameplayEventData EventData)
{
    UWorld* World = GetWorld();
    if (!World) return;

    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
    if (!SkeletalMeshComponent) return;

    const FTransform SocketTransform = SkeletalMeshComponent->GetSocketTransform(SpawnProjectileSocket);
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = Avatar;

    if (!ProjectileClass) return;

    Projectile = World->SpawnActor<AVbProjectile>(ProjectileClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParameters);
    if (!Projectile) return;

    Projectile->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnProjectileSocket);

    UAbilityTask_WaitGameplayEvent* WaitStartProjectileEvent =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_ProjectileStart);
    WaitStartProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_RainOfArrows::OnStartProjectile);
    WaitStartProjectileEvent->ReadyForActivation();
}

void UVbGA_RainOfArrows::OnStartProjectile(FGameplayEventData EventData)
{
    if (!Projectile) return;

    Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Projectile->StartProjectile(Projectile->GetActorRotation().Vector(), FVbAttackContext());

    FGameplayCueParameters CueParams;
    CueParams.Location = TargetActorPoint;

    K2_ExecuteGameplayCueWithParams(VbGameplayTags::GameplayCue_RainOfArrows, CueParams);

    if (K2_HasAuthority())
    {
        UVbCombatComponent* CombatComponent = GetCombatComponent();
        if (!CombatComponent) return;

        AreaTraceInfo.TraceStart = TargetActorPoint;
        AreaTraceInfo.TraceEnd = TargetActorPoint;
        AreaTraceInfo.AttackContext.Damage = UVbFunctionLibrary::GetAbilityDamage(GetWorld(), GetClass()).GetValueAtLevel(GetAbilityLevel());
        AreaTraceInfo.AttackContext.ImpactTypeTag = ImpactTypeTag;
        if (const UVbStyleData* StyleData = GetStyleData())
        {
            AreaTraceInfo.AttackContext.StyleTag = StyleData->GetStyleTag();
        }

        CombatComponent->StartAreaTrace(AreaTraceInfo);
    }
}