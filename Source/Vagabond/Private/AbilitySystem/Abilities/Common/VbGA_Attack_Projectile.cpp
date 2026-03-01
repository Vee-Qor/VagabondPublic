// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/Abilities/Common/VbGA_Attack_Projectile.h"

#include "AIController.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "VbGameplayTags.h"
#include "Framework/VbFunctionLibrary.h"
#include "Weapon/VbProjectile.h"

UVbGA_Attack_Projectile::UVbGA_Attack_Projectile()
{
    SetAssetTags(VbGameplayTags::Ability_Gameplay_Attack_Range_Light.GetTag().GetSingleTagContainer());
}

void UVbGA_Attack_Projectile::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (Projectile)
    {
        Projectile->Destroy();
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVbGA_Attack_Projectile::ExecuteAttack()
{
    Super::ExecuteAttack();

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitSpawnProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
            VbGameplayTags::Event_Attack_ProjectileSpawn);
        WaitSpawnProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_Attack_Projectile::SpawnProjectile);
        WaitSpawnProjectileEvent->ReadyForActivation();
    }
}

FVector UVbGA_Attack_Projectile::GetLaunchDirection() const
{
    FVector Direction = FVector::ZeroVector;
    if (!Avatar) return Direction;

    const AController* Controller = Avatar->GetInstigatorController();
    if (!Controller) return Direction;

    Direction = Controller->GetControlRotation().Vector();

    if (const AAIController* AIController = Cast<AAIController>(Controller))
    {
        if (const UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
        {
            if (const AActor* FocusTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetActor")))
            {
                Direction = FocusTarget->GetActorLocation() - Projectile->GetActorLocation();
            }
        }
    }

    return Direction;
}

void UVbGA_Attack_Projectile::SpawnProjectile(FGameplayEventData EventData)
{
    UWorld* World = GetWorld();
    if (!World) return;
    Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    USkeletalMeshComponent* SkeletalMeshComponent = GetOwningComponentFromActorInfo();
    if (!SkeletalMeshComponent) return;

    const FTransform SocketTransform = SkeletalMeshComponent->GetSocketTransform(SpawnSocket);
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = Avatar;

    if (!ProjectileClass) return;

    Projectile = World->SpawnActor<AVbProjectile>(ProjectileClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParameters);
    if (!Projectile) return;

    Projectile->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnSocket);

    UAbilityTask_WaitGameplayEvent* WaitStartProjectileEvent =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, VbGameplayTags::Event_Attack_ProjectileStart);
    WaitStartProjectileEvent->EventReceived.AddDynamic(this, &UVbGA_Attack_Projectile::OnStartProjectile);
    WaitStartProjectileEvent->ReadyForActivation();
}

void UVbGA_Attack_Projectile::OnStartProjectile(FGameplayEventData EventData)
{
    Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVbAttackContext AttackContext;
    AttackContext.Damage = UVbFunctionLibrary::GetAbilityDamage(GetWorld(), GetClass()).GetValueAtLevel(GetAbilityLevel());
    if (const UVbStyleData* StyleData = GetStyleData())
    {
        AttackContext.StyleTag = StyleData->GetStyleTag();
    }

    Projectile->StartProjectile(GetLaunchDirection(), AttackContext);
    Projectile = nullptr;
}