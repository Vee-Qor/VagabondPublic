// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/VbStyleData.h"
#include "VbWeapon.generated.h"

class UVbAbilitySystemComponent;
class UVbCombatComponent;
class UEnhancedInputLocalPlayerSubsystem;
class UVbStyleData;
struct FGameplayAbilitySpecHandle;

UCLASS()
class AVbWeapon : public AActor
{
    GENERATED_BODY()

public:
    AVbWeapon();
    virtual void SetOwner(AActor* NewOwner) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void InitializeStyleDefinition(UVbStyleData* InStyleDefinition);
    void OnEquipped();
    void OnUnequipped();

    // ==== Accessors ==== //
    FORCEINLINE UVbStyleData* GetStyleData() const { return StyleData; }

private:
    UFUNCTION()
    void OnRep_StyleDefinition();

    UStaticMeshComponent* CreateMeshComponent(const FName& Name);
    void AttachToSocket(UStaticMeshComponent* MeshComponent, const FName& SocketName) const;
    void ConfigureMeshComponent(UStaticMeshComponent*& MeshComponent, const FVbWeaponVisual& WeaponVisual) const;

    void GrantAbilities();
    void RemoveAbilities();
    void LinkAnimLayer() const;
    void UnlinkAnimLayer() const;
    void AddInputMapping() const;
    void RemoveInputMapping() const;

    // ==== Utils ==== //
    UEnhancedInputLocalPlayerSubsystem* GetLocalInputSubsystem() const;
    UVbAbilitySystemComponent* GetAbilitySystemComponent() const;
    UAnimInstance* GetOwnerAnimInstance() const;
    UVbCombatComponent* GetCombatComponent() const;
    bool IsLocallyPlayerControlled() const;

    // ==== Components ==== //
    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* MainHandMesh;

    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* MainAccessoryMesh;

    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* OffHandMesh;

    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* OffAccessoryMesh;

    // ==== Config ==== //
    UPROPERTY(ReplicatedUsing=OnRep_StyleDefinition)
    UVbStyleData* StyleData;

    // ==== Runtime ==== //
    UPROPERTY()
    ACharacter* OwnerCharacter;

    UPROPERTY()
    USkeletalMeshComponent* OwnerMesh;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> GrantedWeaponAbilityHandles;

    UPROPERTY()
    FVbWeaponVisualConfig VisualConfig;
};