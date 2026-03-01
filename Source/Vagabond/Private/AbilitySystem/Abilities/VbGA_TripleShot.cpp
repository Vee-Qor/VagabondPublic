// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/VbGA_TripleShot.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Common/VbGA_Attack_Projectile.h"
#include "Framework/VbFunctionLibrary.h"
#include "VbGameplayTags.h"
#include "Weapon/VbProjectile.h"

UVbGA_TripleShot::UVbGA_TripleShot()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_TripleShot.GetTag().GetSingleTagContainer());

    ActivationOwnedTags.AddTag(VbGameplayTags::Action_Attacking);
    BlockAbilitiesWithTag.AddTag(VbGameplayTags::Ability_Gameplay_Attack);
    ActivationBlockedTags.AddTag(VbGameplayTags::State_Combat_Unarmed);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Knockdown);
    ActivationBlockedTags.AddTag(VbGameplayTags::Effect_Grabbed);
    ActivationRequiredTags.AddTag(VbGameplayTags::State_Combat_Armed);

    SetInputTag(VbGameplayTags::Input_AbilityOne);
}

void UVbGA_TripleShot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ensure(TripleShotMontage);

    if (!K2_CommitAbilityCooldown())
    {
        K2_EndAbility();
        return;
    }

    if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;

    UAbilityTask_PlayMontageAndWait* PlayTripleShotMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TripleShotMontage,
        1.0f, NAME_None, false);
    PlayTripleShotMontage->OnCompleted.AddDynamic(this, &UVbGA_TripleShot::K2_EndAbility);
    PlayTripleShotMontage->OnCancelled.AddDynamic(this, &UVbGA_TripleShot::K2_EndAbility);
    PlayTripleShotMontage->OnBlendOut.AddDynamic(this, &UVbGA_TripleShot::K2_EndAbility);
    PlayTripleShotMontage->OnInterrupted.AddDynamic(this, &UVbGA_TripleShot::K2_EndAbility);
    PlayTripleShotMontage->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitAttackEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_End);
    WaitAttackEndEvent->EventReceived.AddDynamic(this, &UVbGA_TripleShot::OnAttackEnd);
    WaitAttackEndEvent->ReadyForActivation();

    UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    WaitInputRelease->OnRelease.AddDynamic(this, &UVbGA_TripleShot::OnInputReleased);
    WaitInputRelease->ReadyForActivation();

    if (K2_HasAuthority())
    {
        StartCharge();
    }

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitSpawnProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
            VbGameplayTags::Event_Attack_ProjectileSpawn);
        WaitSpawnProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_TripleShot::SpawnProjectile);
        WaitSpawnProjectileEvent->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitStartProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
            VbGameplayTags::Event_Attack_ProjectileStart);
        WaitStartProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_TripleShot::StartProjectile);
        WaitStartProjectileEvent->ReadyForActivation();
    }
}

void UVbGA_TripleShot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ChargeTimerHandle);
    }

    for (AVbProjectile* Projectile : SpawnedProjectiles)
    {
        if (!Projectile) continue;
        Projectile->Destroy();
    }

    SpawnedProjectiles.Empty();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FVector UVbGA_TripleShot::GetLaunchDirection() const
{
    FVector Direction = FVector::ZeroVector;
    if (!Avatar) return Direction;

    const AController* Controller = Avatar->GetInstigatorController();
    if (!Controller) return Direction;

    Direction = Controller->GetControlRotation().Vector();

    return Direction;
}

void UVbGA_TripleShot::StartCharge()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UVbGA_TripleShot::UpdateCharge, SpawnProjectileDelay, true);
}

void UVbGA_TripleShot::UpdateCharge()
{
    if (SpawnedProjectiles.Num() >= MaxProjectiles)
    {
        if (const UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(ChargeTimerHandle);
        }

        return;
    }

    if (!K2_CommitAbilityCost())
    {
        return;
    }

    K2_ExecuteGameplayCueWithParams(VbGameplayTags::GameplayCue_TripleShot_Charge, FGameplayCueParameters());

    if (K2_HasAuthority())
    {
        SpawnProjectile(FGameplayEventData());
    }
}

void UVbGA_TripleShot::StopCharge()
{
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ChargeTimerHandle);
    }
}

void UVbGA_TripleShot::OnInputReleased(float TimeHeld)
{
    StopCharge();

    if (UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance())
    {
        if (CurrentMontage)
        {
            AnimInstance->Montage_SetNextSection(AnimInstance->Montage_GetCurrentSection(), EndSectionName, CurrentMontage);
        }
    }
}

void UVbGA_TripleShot::OnAttackEnd(FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UVbGA_TripleShot::SpawnProjectile(FGameplayEventData EventData)
{
    UWorld* World = GetWorld();
    if (!World) return;

    Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
    if (!SkeletalMeshComponent) return;

    const FTransform SocketTransform = SkeletalMeshComponent->GetSocketTransform(SpawnSocket);

    const FVector BaseLoc = SocketTransform.GetLocation();
    const FRotator BaseRot = SocketTransform.GetRotation().Rotator();

    const FVector Right = SocketTransform.GetUnitAxis(EAxis::Y);

    int32 i = SpawnedProjectiles.Num();
    float Offset = 0.f;

    if (i == 0) Offset = 0.0f;
    else if (i == 1) Offset = -ProjectileOffset;
    else if (i == 2) Offset = +ProjectileOffset;

    const FVector SpawnLoc = BaseLoc + Right * Offset;

    FActorSpawnParameters Params;
    Params.Owner = Avatar;

    AVbProjectile* NewProjectile = World->SpawnActor<AVbProjectile>(ProjectileClass, SpawnLoc, BaseRot, Params);
    if (!NewProjectile) return;

    NewProjectile->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepWorldTransform, SpawnSocket);

    SpawnedProjectiles.Add(NewProjectile);
}

void UVbGA_TripleShot::StartProjectile(FGameplayEventData EventData)
{
    UpdateMotionWarpingTarget();

    for (AVbProjectile* Projectile : SpawnedProjectiles)
    {
        if (!Projectile) continue;

        Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        FVbAttackContext AttackContext;
        AttackContext.Damage = UVbFunctionLibrary::GetAbilityDamage(GetWorld(), GetClass()).GetValueAtLevel(GetAbilityLevel());
        if (const UVbStyleData* StyleData = GetStyleData())
        {
            AttackContext.StyleTag = StyleData->GetStyleTag();
        }

        Projectile->StartProjectile(GetLaunchDirection(), AttackContext);
    }

    SpawnedProjectiles.Empty();
}