// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Animation/VbAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimNodeReference.h"
#include "Character/VbCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "SequencePlayerLibrary.h"
#include "VbGameplayTags.h"
#include "Character/VbEnemyCharacter.h"
#include "Data/VbCharacterData.h"

FVbAnimInstanceProxy::FVbAnimInstanceProxy()
{
}

void FVbAnimInstanceProxy::InitializeObjects(UAnimInstance* InAnimInstance)
{
    FAnimInstanceProxy::InitializeObjects(InAnimInstance);

    OwnerCharacter = Cast<AVbCharacter>(InAnimInstance->TryGetPawnOwner());
    if (!OwnerCharacter) return;

    MovementComponent = OwnerCharacter->GetCharacterMovement();

    ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
}

void FVbAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
    FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

    if (!OwnerCharacter || !MovementComponent) return;

    Acceleration = MovementComponent->GetCurrentAcceleration();
    Velocity = OwnerCharacter->GetVelocity();
    ActorRotation = OwnerCharacter->GetActorRotation();
    bIsJumping = MovementComponent->IsFalling() && Velocity.Z > 0.0f;
    bIsFalling = MovementComponent->IsFalling() && Velocity.Z <= 0.0f;
    bIsOnGround = MovementComponent->IsMovingOnGround();
    if (ASC)
    {
        bIsKnockdown = ASC->HasMatchingGameplayTag(VbGameplayTags::Effect_Knockdown);
    }

    if (const UVbAnimInstance* VbAnimInstance = Cast<UVbAnimInstance>(InAnimInstance))
    {
        bShouldUseFootPlacement = !MovementComponent->IsFalling() && !VbAnimInstance->IsAnyMontagePlaying();
    }

    if (const AVbEnemyCharacter* EnemyCharacter = Cast<AVbEnemyCharacter>(OwnerCharacter))
    {
        if (const AActor* TargetActor = EnemyCharacter->GetTargetActor())
        {
            LookAtLocation = TargetActor->GetActorTransform();
        }
    }
}

void FVbAnimInstanceProxy::Update(float DeltaSeconds)
{
    FAnimInstanceProxy::Update(DeltaSeconds);

    Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.0f);

    bHasAcceleration = Acceleration2D.SizeSquared2D() > KINDA_SMALL_NUMBER;

    Velocity2D = FVector(Velocity.X, Velocity.Y, 0.0f);
    VelocitySize2D = Velocity.Size2D();

    bHasVelocity = VelocitySize2D > KINDA_SMALL_NUMBER;

    VelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity2D, ActorRotation);
    MoveCardinal = CalculateMoveCardinal();

    bShouldPlayUpperBody = bHasAcceleration || bHasVelocity || bIsJumping || bIsFalling;
}

void FVbAnimInstanceProxy::PostUpdate(UAnimInstance* InAnimInstance) const
{
    FAnimInstanceProxy::PostUpdate(InAnimInstance);

    UVbAnimInstance* VbAnimInstance = Cast<UVbAnimInstance>(InAnimInstance);
    if (!VbAnimInstance) return;

    VbAnimInstance->Acceleration = Acceleration;
    VbAnimInstance->Acceleration2D = Acceleration2D;
    VbAnimInstance->Velocity = Velocity;
    VbAnimInstance->Velocity2D = Velocity2D;
    VbAnimInstance->VelocitySize2D = VelocitySize2D;
    VbAnimInstance->LookAtLocation = LookAtLocation;
    VbAnimInstance->ActorRotation = ActorRotation;
    VbAnimInstance->bHasAcceleration = bHasAcceleration;
    VbAnimInstance->bHasVelocity = bHasVelocity;
    VbAnimInstance->VelocityDirectionAngle = VelocityDirectionAngle;
    VbAnimInstance->MoveCardinal = MoveCardinal;
    VbAnimInstance->bIsJumping = bIsJumping;
    VbAnimInstance->bIsFalling = bIsFalling;
    VbAnimInstance->bIsOnGround = bIsOnGround;
    VbAnimInstance->bIsKnockdown = bIsKnockdown;
    VbAnimInstance->bShouldUseFootPlacement = bShouldUseFootPlacement;
    VbAnimInstance->bShouldPlayUpperBody = bShouldPlayUpperBody;
}

EVbMoveCardinal FVbAnimInstanceProxy::CalculateMoveCardinal() const
{
    if (UKismetMathLibrary::InRange_FloatFloat(VelocityDirectionAngle, -45, 45))
    {
        return EVbMoveCardinal::Forward;
    }

    if (!UKismetMathLibrary::InRange_FloatFloat(VelocityDirectionAngle, -135, 135))
    {
        return EVbMoveCardinal::Backward;
    }

    if (VelocityDirectionAngle < 0.0f)
    {
        return EVbMoveCardinal::Left;
    }

    if (VelocityDirectionAngle > 0.0f)
    {
        return EVbMoveCardinal::Right;
    }

    return EVbMoveCardinal::Forward;
}

void UVbAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SetKnockdownAnimations();
}

void UVbAnimInstance::DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy)
{

}

bool UVbAnimInstance::HasOwnerTag(struct FGameplayTag TagToCheck) const
{
    if (AnimProxy.ASC)
    {
        return AnimProxy.ASC->HasMatchingGameplayTag(TagToCheck);
    }

    return false;
}

void UVbAnimInstance::SetKnockdownAnimations()
{
    if (AVbCharacter* Character = Cast<AVbCharacter>(TryGetPawnOwner()))
    {
        if (UVbCharacterData* CharacterData = Character->GetCharacterData())
        {
            FVbKnockdownAnimationSet KnockdownAnimationSet = CharacterData->GetKnockdownAnimationSet();

            KnockdownStartSequence = KnockdownAnimationSet.KnockdownStart;
            KnockdownLoopSequence = KnockdownAnimationSet.KnockdownLoop;
            KnockdownLandSequence = KnockdownAnimationSet.KnockdownLand;
            KnockdownRecoverySequence = KnockdownAnimationSet.KnockdownRecovery;
        }
    }
}

void UVbAnimInstance::UpdateIdleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node) const
{
    if (!IdleSequence) return;

    EAnimNodeReferenceConversionResult ConvResult;
    const FSequencePlayerReference PlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConvResult);
    if (ConvResult == EAnimNodeReferenceConversionResult::Failed) return;

    USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, PlayerRef, IdleSequence);
}

void UVbAnimInstance::UpdateCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node) const
{
    EAnimNodeReferenceConversionResult ConvResult;
    const FSequencePlayerReference PlayerRef = USequencePlayerLibrary::ConvertToSequencePlayer(Node, ConvResult);
    if (ConvResult == EAnimNodeReferenceConversionResult::Failed) return;

    UAnimSequenceBase* Cycle = nullptr;

    switch (MoveCardinal)
    {
        case EVbMoveCardinal::Forward: Cycle = CycleSequences.Forward;
            break;
        case EVbMoveCardinal::Backward: Cycle = CycleSequences.Backward;
            break;
        case EVbMoveCardinal::Left: Cycle = CycleSequences.Left;
            break;
        case EVbMoveCardinal::Right: Cycle = CycleSequences.Right;
            break;
    }

    if (!Cycle) return;

    USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, PlayerRef, Cycle);
}