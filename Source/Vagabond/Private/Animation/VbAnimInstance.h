// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "VbAnimInstance.generated.h"

class UAbilitySystemComponent;
class AVbCharacter;
class UCharacterMovementComponent;
struct FAnimUpdateContext;
struct FAnimNodeReference;

UENUM(BlueprintType)
enum class EVbMoveCardinal : uint8
{
    Forward,
    Backward,
    Left,
    Right
};

USTRUCT(BlueprintType)
struct FVbCardinalSequence
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* Forward = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* Backward = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* Left = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* Right = nullptr;
};

USTRUCT()
struct FVbAnimInstanceProxy : public FAnimInstanceProxy
{
    GENERATED_BODY()

    FVbAnimInstanceProxy();

    virtual void InitializeObjects(UAnimInstance* InAnimInstance) override;
    virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
    virtual void Update(float DeltaSeconds) override;
    virtual void PostUpdate(UAnimInstance* InAnimInstance) const override;

    UPROPERTY(Transient)
    AVbCharacter* OwnerCharacter = nullptr;

    UPROPERTY(Transient)
    UCharacterMovementComponent* MovementComponent = nullptr;

    UPROPERTY(Transient)
    UAbilitySystemComponent* ASC = nullptr;

    UPROPERTY(Transient)
    bool bHasAcceleration = false;

    UPROPERTY(Transient)
    bool bHasVelocity = false;

    UPROPERTY(Transient)
    FVector Acceleration = FVector::ZeroVector;

    UPROPERTY(Transient)
    FVector Acceleration2D = FVector::ZeroVector;

    UPROPERTY(Transient)
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY(Transient)
    FVector Velocity2D = FVector::ZeroVector;

    UPROPERTY(Transient)
    FTransform LookAtLocation = FTransform::Identity;

    UPROPERTY(Transient)
    float VelocitySize2D = 0.0f;

    UPROPERTY(Transient)
    FRotator ActorRotation = FRotator::ZeroRotator;

    UPROPERTY(Transient)
    float VelocityDirectionAngle = 0.0f;

    UPROPERTY(Transient)
    bool bIsJumping = false;

    UPROPERTY(Transient)
    bool bIsFalling = false;

    UPROPERTY(Transient)
    bool bIsOnGround = false;
    
    UPROPERTY(Transient)
    bool bIsKnockdown = false;

    UPROPERTY(Transient)
    bool bShouldUseFootPlacement = false;
    
    UPROPERTY(Transient)
    bool bShouldPlayUpperBody = false;

    UPROPERTY(Transient)
    EVbMoveCardinal MoveCardinal = EVbMoveCardinal::Forward;

    EVbMoveCardinal CalculateMoveCardinal() const;
};

UCLASS()
class UVbAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
public:
    virtual void NativeInitializeAnimation() override;

private:
    virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &AnimProxy; }
    virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;

    UFUNCTION(BlueprintCallable, BlueprintPure, meta=(BlueprintThreadSafe))
    bool HasOwnerTag(struct FGameplayTag TagToCheck) const;
    
    void SetKnockdownAnimations();

    UPROPERTY(Transient)
    FVbAnimInstanceProxy AnimProxy;

    friend struct FVbAnimInstanceProxy;

    UPROPERTY(BlueprintReadOnly, Category="Acceleration", meta=(AllowPrivateAccess="true"))
    bool bHasAcceleration;

    UPROPERTY(BlueprintReadOnly, Category="Acceleration", meta=(AllowPrivateAccess="true"))
    bool bHasVelocity;

    UPROPERTY(BlueprintReadOnly, Category="Acceleration", meta=(AllowPrivateAccess="true"))
    FVector Acceleration;

    UPROPERTY(BlueprintReadOnly, Category="Acceleration", meta=(AllowPrivateAccess="true"))
    FVector Acceleration2D;

    UPROPERTY(BlueprintReadOnly, Category="Velocity", meta=(AllowPrivateAccess="true"))
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category="Velocity", meta=(AllowPrivateAccess="true"))
    FVector Velocity2D;

    UPROPERTY(BlueprintReadOnly, Category="Look", meta=(AllowPrivateAccess="true"))
    FTransform LookAtLocation = FTransform::Identity;

    UPROPERTY(BlueprintReadOnly, Category="Velocity", meta=(AllowPrivateAccess="true"))
    float VelocitySize2D;

    UPROPERTY(BlueprintReadOnly, Category="Rotation", meta=(AllowPrivateAccess="true"))
    FRotator ActorRotation;

    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    float VelocityDirectionAngle;

    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bIsJumping;

    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bIsFalling;

    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bIsOnGround; 
    
    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bIsKnockdown;

    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bShouldUseFootPlacement; 
    
    UPROPERTY(BlueprintReadOnly, Category="Locomotion", meta=(AllowPrivateAccess="true"))
    bool bShouldPlayUpperBody;

    UPROPERTY(BlueprintReadOnly, Category="CardinalDirection", meta=(AllowPrivateAccess="true"))
    EVbMoveCardinal MoveCardinal = EVbMoveCardinal::Forward;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* IdleSequence;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* JumpStartSequence;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* JumpLoopSequence;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* JumpEndSequence;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    FVbCardinalSequence CycleSequences;
    
    UPROPERTY(BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* KnockdownStartSequence;
    
    UPROPERTY(BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* KnockdownLoopSequence;

    UPROPERTY(BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* KnockdownLandSequence;
    
    UPROPERTY(BlueprintReadOnly, Category="Animation", meta=(AllowPrivateAccess="true"))
    UAnimSequenceBase* KnockdownRecoverySequence;

    UFUNCTION(BlueprintCallable, Category="AnimGraph", meta=(BlueprintThreadSafe))
    void UpdateIdleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node) const;

    UFUNCTION(BlueprintCallable, Category="AnimGraph", meta=(BlueprintThreadSafe))
    void UpdateCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node) const;
};