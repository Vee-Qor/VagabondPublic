// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "VbCharacter.generated.h"

class UVbAttributeSet;
class UVbAbilitySystemComponent;
class UVbCombatComponent;
class UVbCharacterData;
class UVbEquipmentComponent;
class UVbFootstepComponent;
class UVbInventoryComponent;
class UVbInteractionComponent;
class UMotionWarpingComponent;
struct FGameplayTag;
struct FGameplayEventData;
struct FOnAttributeChangeData;

UCLASS()
class AVbCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AVbCharacter();

    void LinkDefaultAnimLayer() const;

    UFUNCTION(Client, Reliable)
    void Client_SendGameplayEvent(AActor* Actor, const FGameplayTag& EventTag, const FGameplayEventData& EventData) const;

    // ==== Death ==== //
    UFUNCTION(Server, Reliable)
    void Server_RequestRespawn();

    // ==== Accessors ==== //
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    FORCEINLINE virtual UVbAbilitySystemComponent* GetVbAbilitySystemComponent() const { return VbAbilitySystemComponent; }
    FORCEINLINE virtual UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
    FORCEINLINE UVbInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
    FORCEINLINE UVbInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
    FORCEINLINE FTransform GetMeshRelativeTransform() const { return MeshRelativeTransform; }
    FORCEINLINE void SetVbAbilitySystemComponent(UVbAbilitySystemComponent* InASC) { VbAbilitySystemComponent = InASC; }
    FORCEINLINE void SetVbAttributeSet(UVbAttributeSet* InAttributeSet) { VbAttributeSet = InAttributeSet; }
    void SetGrabContext(AActor* Actor, FName Socket);

    UFUNCTION(BlueprintPure)
    FORCEINLINE UVbCharacterData* GetCharacterData() const { return CharacterData; }

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void InitializeAbilityActorInfo();
    void InitializeDefault();

    // ==== Death ==== //
    virtual void OnDying(const FGameplayTag Tag, int32 NewCount);
    virtual void OnRespawning(const FGameplayTag Tag, int32 NewCount);
    virtual void OnGrabbed(const FGameplayTag Tag, int32 NewCount);
    virtual void OnKnockdown(const FGameplayTag Tag, int32 NewCount);
    virtual void OnStun(const FGameplayTag Tag, int32 NewCount);

private:
    void SetGrabEnabled(bool bEnabled);
    void SendHitReactEvent(const FOnAttributeChangeData& ChangeData);
    void SendDeathEvent(const FOnAttributeChangeData& ChangeData);
    void SetRagdollEnabled(bool bEnabled) const;
    void SetPerceptionSourceEnabled(bool bEnabled);
    void SetLooseTagState(const FGameplayTag& StateTag, bool bEnabled) const;
    void LinkRigAnimLayer() const;
    void UpdateFullEmptyTags(float CurrentValue, float MaxValue, const FGameplayTag& FullTag, const FGameplayTag& EmptyTag) const;

    // ==== Taken Damage ==== //
    void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
    void OnResourceChanged(const FOnAttributeChangeData& ChangeData);
    void OnPoiseChanged(const FOnAttributeChangeData& ChangeData);
    void OnDead(const FGameplayTag Tag, int32 NewCount);

    // ==== Components ==== //
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
    UVbAbilitySystemComponent* VbAbilitySystemComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
    UVbAttributeSet* VbAttributeSet;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
    UVbCombatComponent* CombatComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    UVbFootstepComponent* FootStepComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    UMotionWarpingComponent* MotionWarpingComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    UVbInteractionComponent* InteractionComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="ItemSystem", meta=(AllowPrivateAccess="true"))
    UVbInventoryComponent* InventoryComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="ItemSystem", meta=(AllowPrivateAccess="true"))
    UVbEquipmentComponent* EquipmentComponent;

    // ==== Config ==== //
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CharacterInfo", meta=(AllowPrivateAccess="true"))
    UVbCharacterData* CharacterData;

    // ==== Runtime ==== //
    UPROPERTY()
    FTransform MeshRelativeTransform;

    UPROPERTY(Replicated)
    AActor* GrabberActor;

    UPROPERTY(Replicated)
    FName GrabberSocket;
};