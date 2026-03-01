// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAttributeSet.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/VbCombatComponent.h"
#include "Character/VbInteractionComponent.h"
#include "Data/VbCharacterData.h"
#include "VbFootstepComponent.h"
#include "VbGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ItemSystem/VbInventoryComponent.h"
#include "ItemSystem/VbEquipmentComponent.h"
#include "MotionWarpingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

AVbCharacter::AVbCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    GetCharacterMovement()->SetWalkableFloorAngle(60.0f);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_MeleeTrace, ECR_Block);
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    CombatComponent = CreateDefaultSubobject<UVbCombatComponent>("CombatComponent");
    FootStepComponent = CreateDefaultSubobject<UVbFootstepComponent>("FootStepComponent");
    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");

    InteractionComponent = CreateDefaultSubobject<UVbInteractionComponent>("InteractionComponent");

    InventoryComponent = CreateDefaultSubobject<UVbInventoryComponent>("InventoryComponent");
    InventoryComponent->SetIsReplicated(true);

    EquipmentComponent = CreateDefaultSubobject<UVbEquipmentComponent>("EquipmentComponent");
    EquipmentComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* AVbCharacter::GetAbilitySystemComponent() const
{
    return GetVbAbilitySystemComponent();
}

void AVbCharacter::SetGrabContext(AActor* Actor, FName Socket)
{
    if (!HasAuthority()) return;

    GrabberActor = Actor;
    GrabberSocket = Socket;
}

void AVbCharacter::BeginPlay()
{
    Super::BeginPlay();

    LinkRigAnimLayer();

    if (GetMesh())
    {
        MeshRelativeTransform = GetMesh()->GetRelativeTransform();
    }
}

void AVbCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbCharacter, GrabberActor);
    DOREPLIFETIME(AVbCharacter, GrabberSocket);
}

void AVbCharacter::InitializeAbilityActorInfo()
{
    InitializeDefault();
}

void AVbCharacter::InitializeDefault()
{
    if (!VbAbilitySystemComponent || !CharacterData) return;

    if (CombatComponent)
    {
        CombatComponent->Initialize(VbAbilitySystemComponent, CharacterData->GetDefaultStyles());
    }

    if (VbAbilitySystemComponent)
    {
        VbAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetHealthAttribute()).AddUObject(this,
            &AVbCharacter::OnHealthChanged);
        VbAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetResourceAttribute()).AddUObject(this,
            &AVbCharacter::OnResourceChanged);
        VbAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetPoiseAttribute()).AddUObject(this, &AVbCharacter::OnPoiseChanged);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::State_Dying).AddUObject(this, &AVbCharacter::OnDying);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::State_Dead).AddUObject(this, &AVbCharacter::OnDead);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::State_Respawning).AddUObject(this, &AVbCharacter::OnRespawning);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::Effect_Grabbed).AddUObject(this, &AVbCharacter::OnGrabbed);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::Effect_Knockdown).AddUObject(this, &AVbCharacter::OnKnockdown);
        VbAbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::Effect_Stun).AddUObject(this, &AVbCharacter::OnStun);
    }

    if (HasAuthority() && CharacterData)
    {
        VbAbilitySystemComponent->InitializeDefaultAttributes(CharacterData);

        VbAbilitySystemComponent->ApplyEffects(CharacterData->GetAbilityConfig().Effects);
        VbAbilitySystemComponent->AddAbilities(CharacterData->GetAbilityConfig().Abilities);
        VbAbilitySystemComponent->AddAbilities(CharacterData->GetAbilityConfig().PassiveAbilities);
    }
}

void AVbCharacter::SetGrabEnabled(bool bEnabled)
{
    if (!GetMesh() || !GetRootComponent()) return;

    if (bEnabled)
    {
        if (!GrabberActor || GrabberSocket.IsNone()) return;

        const ACharacter* GrabberCharacter = Cast<ACharacter>(GrabberActor);
        if (!GrabberCharacter) return;

        AttachToComponent(GrabberCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GrabberSocket);
    }
    else
    {
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
        GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        GetMesh()->SetRelativeTransform(MeshRelativeTransform);

        SetGrabContext(nullptr, NAME_None);
    }
}

void AVbCharacter::SendHitReactEvent(const FOnAttributeChangeData& ChangeData)
{
    if (!ChangeData.GEModData) return;

    FGameplayEventData EventData;
    EventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
    EventData.InstigatorTags = ChangeData.GEModData->EffectSpec.GetDynamicAssetTags();
    EventData.EventMagnitude = ChangeData.OldValue - ChangeData.NewValue;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, VbGameplayTags::Event_Ability_HitReact, EventData);
}

void AVbCharacter::SendDeathEvent(const FOnAttributeChangeData& ChangeData)
{
    if (!CharacterData || !ChangeData.GEModData) return;

    const TArray<UAnimMontage*>& DeathMontages = CharacterData->GetDeathMontages();
    if (DeathMontages.IsEmpty()) return;

    FGameplayEventData EventData;
    EventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
    EventData.EventMagnitude = FMath::RandRange(0, DeathMontages.Num() - 1);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, VbGameplayTags::Event_Ability_Death, EventData);
}

void AVbCharacter::LinkRigAnimLayer() const
{
    if (GetMesh() && CharacterData && CharacterData->GetRigAnimLayerClass())
    {
        GetMesh()->LinkAnimClassLayers(CharacterData->GetRigAnimLayerClass());
    }
}

void AVbCharacter::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
    if (!HasAuthority() || !VbAbilitySystemComponent) return;

    bool bFound = false;
    const float MaxHealth = VbAbilitySystemComponent->GetGameplayAttributeValue(UVbAttributeSet::GetMaxHealthAttribute(), bFound);
    if (bFound)
    {
        UpdateFullEmptyTags(ChangeData.NewValue, MaxHealth, VbGameplayTags::State_Health_Full, VbGameplayTags::State_Health_Empty);
    }

    if (ChangeData.NewValue < ChangeData.OldValue)
    {
        SendHitReactEvent(ChangeData);
    }

    if (ChangeData.NewValue <= 0.0f)
    {
        SendDeathEvent(ChangeData);
    }
}

void AVbCharacter::OnResourceChanged(const FOnAttributeChangeData& ChangeData)
{
    if (!HasAuthority() || !VbAbilitySystemComponent) return;

    bool bFound = false;
    const float MaxResource = VbAbilitySystemComponent->GetGameplayAttributeValue(UVbAttributeSet::GetMaxResourceAttribute(), bFound);
    if (bFound)
    {
        UpdateFullEmptyTags(ChangeData.NewValue, MaxResource, VbGameplayTags::State_Resource_Full, VbGameplayTags::State_Resource_Empty);
    }
}

void AVbCharacter::OnPoiseChanged(const FOnAttributeChangeData& ChangeData)
{
    if (!HasAuthority() || !VbAbilitySystemComponent) return;

    bool bFound = false;
    const float MaxPoise = VbAbilitySystemComponent->GetGameplayAttributeValue(UVbAttributeSet::GetMaxPoiseAttribute(), bFound);
    if (bFound)
    {
        UpdateFullEmptyTags(ChangeData.NewValue, MaxPoise, VbGameplayTags::State_Poise_Full, VbGameplayTags::State_Poise_Empty);
    }
}

void AVbCharacter::OnDying(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        SetPerceptionSourceEnabled(false);
    }
}

void AVbCharacter::OnRespawning(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        SetRagdollEnabled(false);
        SetPerceptionSourceEnabled(true);
    }
}

void AVbCharacter::OnDead(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        SetRagdollEnabled(true);
    }
}

void AVbCharacter::UpdateFullEmptyTags(float CurrentValue, float MaxValue, const FGameplayTag& FullTag, const FGameplayTag& EmptyTag) const
{
    if (!HasAuthority() || !VbAbilitySystemComponent) return;

    SetLooseTagState(FullTag, CurrentValue >= MaxValue);
    SetLooseTagState(EmptyTag, CurrentValue <= 0.0f);
}

void AVbCharacter::OnGrabbed(const FGameplayTag Tag, int32 NewCount)
{
    const bool bIsGrabbed = NewCount > 0;

    if (!CharacterData) return;

    UAnimMontage* GrabbedMontage = CharacterData->GetGrabbedMontage();
    if (!GrabbedMontage) return;

    SetGrabEnabled(bIsGrabbed);

    if (bIsGrabbed)
    {
        PlayAnimMontage(GrabbedMontage);
    }
    else
    {
        StopAnimMontage(GrabbedMontage);
    }

    GetCapsuleComponent()->SetCollisionEnabled(bIsGrabbed ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void AVbCharacter::OnKnockdown(const FGameplayTag Tag, int32 NewCount)
{
    const bool bIsKnocked = NewCount > 0;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bUseControllerDesiredRotation = !bIsKnocked;
    }
}

void AVbCharacter::OnStun(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        if (VbAbilitySystemComponent)
        {
            FGameplayTagContainer CancelTagContainer;
            CancelTagContainer.AddTag(VbGameplayTags::Ability_Gameplay);

            VbAbilitySystemComponent->CancelAbilities(&CancelTagContainer);
        }
    }
}

void AVbCharacter::Server_RequestRespawn_Implementation()
{
    if (!CharacterData) return;

    const TArray<UAnimMontage*>& RespawnMontages = CharacterData->GetRespawnMontages();
    if (RespawnMontages.IsEmpty()) return;

    FGameplayEventData EventData;
    EventData.EventMagnitude = FMath::RandRange(0, RespawnMontages.Num() - 1);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, VbGameplayTags::Event_Ability_Respawn, EventData);
}

void AVbCharacter::LinkDefaultAnimLayer() const
{
    if (!CharacterData) return;

    if (GetMesh() && CharacterData->GetAbilityConfig().AnimLayerClass)
    {
        GetMesh()->LinkAnimClassLayers(CharacterData->GetAbilityConfig().AnimLayerClass);
    }
}

void AVbCharacter::Client_SendGameplayEvent_Implementation(AActor* Actor, const FGameplayTag& EventTag, const FGameplayEventData& EventData) const
{
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventTag, EventData);
}

void AVbCharacter::SetRagdollEnabled(bool bEnabled) const
{
    if (!GetMesh() || !GetCapsuleComponent()) return;

    GetCapsuleComponent()->SetCollisionEnabled(bEnabled ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::QueryOnly);
    GetMesh()->SetSimulatePhysics(bEnabled);

    if (!bEnabled)
    {
        GetMesh()->SetRelativeTransform(MeshRelativeTransform);
    }
}

void AVbCharacter::SetPerceptionSourceEnabled(bool bEnabled)
{
    if (!HasAuthority()) return;

    UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
    if (!PerceptionSystem) return;

    if (bEnabled)
    {
        PerceptionSystem->RegisterSource(*this);
    }
    else
    {
        PerceptionSystem->UnregisterSource(*this, UAISense_Sight::StaticClass());
        PerceptionSystem->UnregisterSource(*this, UAISense_Hearing::StaticClass());
        PerceptionSystem->UnregisterSource(*this, UAISense_Damage::StaticClass());
    }
}

void AVbCharacter::SetLooseTagState(const FGameplayTag& StateTag, bool bEnabled) const
{
    if (!VbAbilitySystemComponent) return;

    const bool bHas = VbAbilitySystemComponent->HasMatchingGameplayTag(StateTag);
    if (bEnabled && !bHas)
    {
        VbAbilitySystemComponent->AddLooseGameplayTag(StateTag);
    }
    else if (!bEnabled && bHas)
    {
        VbAbilitySystemComponent->RemoveLooseGameplayTag(StateTag);
    }
}