// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Weapon/VbWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Character/VbCombatComponent.h"
#include "Data/VbStyleData.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Framework/VbFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/VbInputConfig.h"
#include "Net/UnrealNetwork.h"

AVbWeapon::AVbWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

    MainHandMesh = CreateMeshComponent("MainHandMesh");
    MainAccessoryMesh = CreateMeshComponent("MainAccessoryMesh");
    OffHandMesh = CreateMeshComponent("OffHandMesh");
    OffAccessoryMesh = CreateMeshComponent("OffAccessoryMesh");
}

void AVbWeapon::InitializeStyleDefinition(UVbStyleData* InStyleDefinition)
{
    if (InStyleDefinition)
    {
        StyleData = InStyleDefinition;
        VisualConfig = StyleData->GetVisualConfig();

        if (GetRootComponent() && OwnerMesh)
        {
            GetRootComponent()->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, VisualConfig.MainHand.EquippedSocket);
        }

        ConfigureMeshComponent(MainHandMesh, VisualConfig.MainHand);
        ConfigureMeshComponent(MainAccessoryMesh, VisualConfig.MainAccessoryHand);
        ConfigureMeshComponent(OffHandMesh, VisualConfig.OffHand);
        ConfigureMeshComponent(OffAccessoryMesh, VisualConfig.OffAccessoryHand);
    }
}

void AVbWeapon::AttachToSocket(UStaticMeshComponent* MeshComponent, const FName& SocketName) const
{
    if (OwnerMesh && MeshComponent)
    {
        MeshComponent->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
    }
}

UStaticMeshComponent* AVbWeapon::CreateMeshComponent(const FName& Name)
{
    UStaticMeshComponent* StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(Name);
    StaticMesh->SetupAttachment(GetRootComponent());
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    return StaticMesh;
}

void AVbWeapon::ConfigureMeshComponent(UStaticMeshComponent*& MeshComponent, const FVbWeaponVisual& WeaponVisual) const
{
    if (!MeshComponent) return;

    if (WeaponVisual.Mesh)
    {
        MeshComponent->SetStaticMesh(WeaponVisual.Mesh);
    }
    else
    {
        MeshComponent->DestroyComponent();
        MeshComponent = nullptr;
    }
}

void AVbWeapon::GrantAbilities()
{
    UVbAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !StyleData) return;

    GrantedWeaponAbilityHandles = ASC->AddAbilities(StyleData->GetAbilityConfig().Abilities);
}

void AVbWeapon::RemoveAbilities()
{
    if (GrantedWeaponAbilityHandles.IsEmpty()) return;

    UVbAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    ASC->RemoveAbilities(GrantedWeaponAbilityHandles);

    GrantedWeaponAbilityHandles.Empty();
}

void AVbWeapon::LinkAnimLayer() const
{
    UAnimInstance* AnimInstance = GetOwnerAnimInstance();
    if (!AnimInstance || !StyleData) return;

    if (const TSubclassOf<UAnimInstance> StyleAnimLayerClass = StyleData->GetAbilityConfig().AnimLayerClass)
    {
        AnimInstance->LinkAnimClassLayers(StyleAnimLayerClass);
    }
}

void AVbWeapon::UnlinkAnimLayer() const
{
    UAnimInstance* AnimInstance = GetOwnerAnimInstance();
    if (!AnimInstance || !StyleData) return;

    if (const TSubclassOf<UAnimInstance> StyleAnimLayerClass = StyleData->GetAbilityConfig().AnimLayerClass)
    {
        AnimInstance->UnlinkAnimClassLayers(StyleAnimLayerClass);
    }
}

void AVbWeapon::AddInputMapping() const
{
    if (!OwnerCharacter) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalInputSubsystem();
    if (!Subsystem) return;

    const UVbInputConfig* InputConfig = UVbFunctionLibrary::GetInputConfig(OwnerCharacter);
    if (!InputConfig) return;

    const UInputMappingContext* CombatMappingContext = InputConfig->GetCombatMappingContext();
    if (!CombatMappingContext) return;

    Subsystem->RemoveMappingContext(CombatMappingContext);
    Subsystem->AddMappingContext(CombatMappingContext, 1);
}

void AVbWeapon::RemoveInputMapping() const
{
    if (!OwnerCharacter) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalInputSubsystem();
    if (!Subsystem) return;

    const UVbInputConfig* InputConfig = UVbFunctionLibrary::GetInputConfig(OwnerCharacter);
    if (!InputConfig) return;

    const UInputMappingContext* CombatMappingContext = InputConfig->GetCombatMappingContext();
    if (!CombatMappingContext) return;

    Subsystem->RemoveMappingContext(CombatMappingContext);
}

void AVbWeapon::OnEquipped()
{
    if (HasAuthority())
    {
        GrantAbilities();
    }

    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
        OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
    }

    AttachToSocket(MainHandMesh, VisualConfig.MainHand.EquippedSocket);
    AttachToSocket(MainAccessoryMesh, VisualConfig.MainAccessoryHand.EquippedSocket);
    AttachToSocket(OffHandMesh, VisualConfig.OffHand.EquippedSocket);
    AttachToSocket(OffAccessoryMesh, VisualConfig.OffAccessoryHand.EquippedSocket);

    LinkAnimLayer();

    if (IsLocallyPlayerControlled())
    {
        AddInputMapping();
    }
}

void AVbWeapon::OnUnequipped()
{
    if (HasAuthority())
    {
        RemoveAbilities();
    }

    if (OwnerCharacter)
    {
        OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
    }

    AttachToSocket(MainHandMesh, VisualConfig.MainHand.UnequippedSocket);
    AttachToSocket(MainAccessoryMesh, VisualConfig.MainAccessoryHand.UnequippedSocket);
    AttachToSocket(OffHandMesh, VisualConfig.OffHand.UnequippedSocket);
    AttachToSocket(OffAccessoryMesh, VisualConfig.OffAccessoryHand.UnequippedSocket);

    UnlinkAnimLayer();

    if (IsLocallyPlayerControlled())
    {
        RemoveInputMapping();
    }
}

void AVbWeapon::SetOwner(AActor* NewOwner)
{
    Super::SetOwner(NewOwner);

    OwnerCharacter = Cast<ACharacter>(NewOwner);
    if (OwnerCharacter)
    {
        OwnerMesh = OwnerCharacter->GetMesh();
    }
}

void AVbWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbWeapon, StyleData);
}

UEnhancedInputLocalPlayerSubsystem* AVbWeapon::GetLocalInputSubsystem() const
{
    if (!OwnerCharacter) return nullptr;

    const APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PlayerController) return nullptr;

    const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

    return LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

bool AVbWeapon::IsLocallyPlayerControlled() const
{
    if (OwnerCharacter)
    {
        return OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsPlayerControlled();
    }

    return false;
}

void AVbWeapon::OnRep_StyleDefinition()
{
    InitializeStyleDefinition(StyleData);
}

UVbAbilitySystemComponent* AVbWeapon::GetAbilitySystemComponent() const
{
    return Cast<UVbAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter));
}

UAnimInstance* AVbWeapon::GetOwnerAnimInstance() const
{
    return OwnerMesh ? OwnerMesh->GetAnimInstance() : nullptr;
}

UVbCombatComponent* AVbWeapon::GetCombatComponent() const
{
    return OwnerCharacter ? OwnerCharacter->FindComponentByClass<UVbCombatComponent>() : nullptr;
}