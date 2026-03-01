// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "VbCoreTypes.h"
#include "Character/VbCharacter.h"
#include "Data/VbStyleData.h"
#include "Framework/VbGameMode.h"
#include "VbGameplayTags.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/VbWeapon.h"

UVbCombatComponent::UVbCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UVbCombatComponent::Initialize(UVbAbilitySystemComponent* ASC, const TArray<UVbStyleData*>& DefaultStyles)
{
    if (!ASC) return;

    AbilitySystemComponent = ASC;

    AbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::State_Combat_Armed).AddUObject(this, &UVbCombatComponent::OnArmedStateTagChanged);
    AbilitySystemComponent->RegisterGameplayTagEvent(VbGameplayTags::State_Combat_Unarmed).AddUObject(this, &UVbCombatComponent::OnArmedStateTagChanged);

    OnAttackHitDetected.AddUObject(this, &UVbCombatComponent::ApplyEffectOnHit);

    //TODO::Only For AI
    if (const ACharacter* OwnerActor = Cast<ACharacter>(GetOwner()))
    {
        if (!OwnerActor->IsPlayerControlled())
        {
            if (!DefaultStyles.IsEmpty())
            {
                for (UVbStyleData* Style : DefaultStyles)
                {
                    if (!Style) continue;

                    StyleSet.Add(Style);
                }
            }

            for (UVbStyleData* Style : StyleSet)
            {
                if (!Style) continue;

                SpawnAndSetWeapon(Style);
                break;
            }
        }
    }

    UpdateWeaponState();
}

void UVbCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UVbCombatComponent, StyleSet);
    DOREPLIFETIME(UVbCombatComponent, CurrentWeapon);
}

bool UVbCombatComponent::IsInFrontArc(const AActor* Target, const FVector& SourceLocation, float AngleDegrees) const
{
    if (!Target) return false;

    FVector ToSource = SourceLocation - Target->GetActorLocation();
    ToSource.Z = 0.0f;

    if (ToSource.IsNearlyZero()) return true;
    ToSource.Normalize();

    FVector Forward = Target->GetActorForwardVector();
    Forward.Z = 0.0f;
    Forward.Normalize();

    const float Dot = FVector::DotProduct(Forward, ToSource);

    const float HalfAngleDegrees = AngleDegrees * 0.5f;
    const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(HalfAngleDegrees));

    return Dot >= CosHalfAngle;
}

bool UVbCombatComponent::ShouldIgnoreHit(const UAbilitySystemComponent* SourceASC, const UAbilitySystemComponent* TargetASC) const
{
    if (!SourceASC || !TargetASC) return true;

    if (TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Damage_Immune)) return true;
    if (TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Dying)) return true;
    if (TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Dead)) return true;
    if (TargetASC->HasMatchingGameplayTag(VbGameplayTags::State_Respawning)) return true;

    const bool bTargetIsEvading = TargetASC->HasMatchingGameplayTag(VbGameplayTags::Effect_Evading);
    const bool bSourceIsUnavoidable = SourceASC->HasMatchingGameplayTag(VbGameplayTags::Combat_Attack_Property_Unavoidable);
    if (bTargetIsEvading && !bSourceIsUnavoidable) return true;

    return false;
}

bool UVbCombatComponent::CanApplyDamageAfterDefense(AActor* HitActor, const UAbilitySystemComponent* SourceASC, const UAbilitySystemComponent* TargetASC,
    const FVector& InstigatorLocation, float& InOutDamage, bool& bOutWasBlocked) const
{
    if (!HitActor || !SourceASC || !TargetASC) return false;

    bOutWasBlocked = false;

    const bool bSourceIsUnblockable = SourceASC->HasMatchingGameplayTag(VbGameplayTags::Combat_Attack_Property_Unblockable);
    const bool bTargetIsInFront = IsInFrontArc(HitActor, InstigatorLocation);
    if (!bTargetIsInFront || bSourceIsUnblockable) return true;

    const bool bTargetIsParrying = TargetASC->HasMatchingGameplayTag(VbGameplayTags::Effect_Parrying);
    const bool bTargetIsBlocking = TargetASC->HasMatchingGameplayTag(VbGameplayTags::Effect_Blocking);

    if (bTargetIsParrying)
    {
        SendBlockedHitEvent(HitActor);
        return false;
    }

    if (bTargetIsBlocking)
    {
        SendBlockedHitEvent(HitActor);
        InOutDamage *= BlockDamageReduction;
        bOutWasBlocked = true;
    }

    return true;
}

void UVbCombatComponent::SwitchWeapon()
{
    if (GetOwnerRole() != ROLE_Authority) return;
    if (StyleSet.Num() <= 1) return;

    const int32 CurrentWeaponIndex = GetCurrentWeaponIndex();
    if (CurrentWeaponIndex == INDEX_NONE) return;

    const int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % StyleSet.Num();
    UVbStyleData* NextStyle = StyleSet[NextWeaponIndex];
    if (!NextStyle) return;

    SpawnAndSetWeapon(NextStyle);
}

void UVbCombatComponent::SetWeaponStyle(UVbStyleData* NewStyle)
{
    if (!NewStyle) return;

    StyleSet.Add(NewStyle);

    if (!CurrentWeapon)
    {
        SpawnAndSetWeapon(NewStyle);
    }
}

void UVbCombatComponent::RemoveWeaponStyle(UVbStyleData* StyleToRemove)
{
    if (!StyleToRemove) return;

    const bool bWasCurrent = CurrentWeapon && CurrentWeapon->GetStyleData() == StyleToRemove;

    const int32 RemovedCount = StyleSet.RemoveSingleSwap(StyleToRemove);
    if (RemovedCount == 0) return;

    if (!bWasCurrent) return;

    if (TrySpawnNextStyleSet()) return;

    if (CurrentWeapon)
    {
        CurrentWeapon->OnUnequipped();
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
        OnCurrentWeaponChanged.Broadcast(CurrentWeapon);
    }

    UpdateWeaponState();
}

bool UVbCombatComponent::DoSphereTraceStep(const FVbTraceParams& Params, TArray<FHitResult>& OutHitResults, bool& bDidTraceStep) const
{
    OutHitResults.Reset();
    bDidTraceStep = false;

    if (FVector::DistSquared(Params.TraceStart, Params.TraceEnd) < FMath::Square(Params.DistanceBetweenTraces)) return false;
    bDidTraceStep = true;

    const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_MeleeTrace);

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return false;

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerActor);

    if (Params.AdditionalIgnoreActor)
    {
        ActorsToIgnore.Add(Params.AdditionalIgnoreActor);
    }

    const EDrawDebugTrace::Type DrawDebug = Params.bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    const IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(OwnerActor->GetInstigatorController());
    if (!OwnerTeamAgent) return false;

    TArray<FHitResult> HitResults;

    if (UKismetSystemLibrary::SphereTraceMulti(this, Params.TraceStart, Params.TraceEnd, Params.SphereRadius, TraceTypeQuery, false,
        ActorsToIgnore, DrawDebug, HitResults, false))
    {
        CollectUniqueEnemyHits(HitResults, OwnerTeamAgent, OutHitResults, Params.AlreadyHitActors);
    }

    return OutHitResults.Num() > 0;
}

bool UVbCombatComponent::DoBoxTraceStep(const FVbTraceParams& Params, TArray<FHitResult>& OutHitResults, bool& bDidTraceStep) const
{
    OutHitResults.Reset();
    bDidTraceStep = false;

    if (FVector::DistSquared(Params.TraceStart, Params.TraceEnd) < FMath::Square(Params.DistanceBetweenTraces)) return false;
    bDidTraceStep = true;

    const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_MeleeTrace);

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return false;

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerActor);

    if (Params.AdditionalIgnoreActor)
    {
        ActorsToIgnore.Add(Params.AdditionalIgnoreActor);
    }

    const EDrawDebugTrace::Type DrawDebug = Params.bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    const IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(OwnerActor->GetInstigatorController());
    if (!OwnerTeamAgent) return false;

    TArray<FHitResult> HitResults;

    if (UKismetSystemLibrary::BoxTraceMulti(this, Params.TraceStart, Params.TraceEnd, Params.BoxSize / 2, Params.BoxRotation,
        TraceTypeQuery, false, ActorsToIgnore, DrawDebug, HitResults, false))
    {
        CollectUniqueEnemyHits(HitResults, OwnerTeamAgent, OutHitResults, Params.AlreadyHitActors);
    }

    return OutHitResults.Num() > 0;
}

void UVbCombatComponent::OnRep_CurrentWeapon(AVbWeapon* OldWeapon)
{
    if (OldWeapon)
    {
        OldWeapon->OnUnequipped();
    }

    UpdateWeaponState();
    OnCurrentWeaponChanged.Broadcast(CurrentWeapon);
}

void UVbCombatComponent::OnArmedStateTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    UpdateWeaponState();
}

void UVbCombatComponent::SetArmedState(bool bArmed) const
{
    if (GetOwnerRole() != ROLE_Authority) return;
    if (!AbilitySystemComponent) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    const AVbGameMode* GameMode = Cast<AVbGameMode>(World->GetAuthGameMode());
    if (!GameMode) return;

    const TSubclassOf<UGameplayEffect>& ArmedStatusEffectClass = GameMode->GetArmedStatusEffectClass();
    const TSubclassOf<UGameplayEffect>& UnarmedStatusEffectClass = GameMode->GetUnarmedStatusEffectClass();

    AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(ArmedStatusEffectClass, AbilitySystemComponent);
    AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(UnarmedStatusEffectClass, AbilitySystemComponent);

    if (bArmed)
    {
        AbilitySystemComponent->ApplyEffect(ArmedStatusEffectClass);
    }
    else
    {
        AbilitySystemComponent->ApplyEffect(UnarmedStatusEffectClass);
    }
}

void UVbCombatComponent::SpawnAndSetWeapon(UVbStyleData* Style)
{
    if (GetOwnerRole() != ROLE_Authority) return;
    if (!Style) return;

    UWorld* World = GetWorld();
    if (!World) return;

    if (CurrentWeapon)
    {
        CurrentWeapon->OnUnequipped();
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
        OnCurrentWeaponChanged.Broadcast(CurrentWeapon);
    }

    AVbWeapon* NewWeapon = World->SpawnActor<AVbWeapon>();
    if (!NewWeapon) return;

    NewWeapon->SetOwner(GetOwner());
    NewWeapon->InitializeStyleDefinition(Style);
    CurrentWeapon = NewWeapon;
    OnCurrentWeaponChanged.Broadcast(CurrentWeapon);

    UpdateWeaponState();
}

bool UVbCombatComponent::TrySpawnNextStyleSet()
{
    if (StyleSet.IsEmpty()) return false;

    for (UVbStyleData* Style : StyleSet)
    {
        if (!Style) continue;
        SpawnAndSetWeapon(Style);
        return true;
    }

    return false;
}

int32 UVbCombatComponent::GetCurrentWeaponIndex() const
{
    if (CurrentWeapon)
    {
        for (int32 i = 0; i < StyleSet.Num(); ++i)
        {
            if (StyleSet[i] == CurrentWeapon->GetStyleData())
            {
                return i;
            }
        }
    }

    return INDEX_NONE;
}

bool UVbCombatComponent::IsArmedState() const
{
    return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(VbGameplayTags::State_Combat_Armed);
}

void UVbCombatComponent::UpdateWeaponState() const
{
    if (CurrentWeapon)
    {
        if (IsArmedState())
        {
            CurrentWeapon->OnEquipped();
            return;
        }

        CurrentWeapon->OnUnequipped();
    }

    if (const AVbCharacter* Character = Cast<AVbCharacter>(GetOwner()))
    {
        Character->LinkDefaultAnimLayer();
    }
}

TSubclassOf<UGameplayEffect> UVbCombatComponent::GetDamageEffectClass() const
{
    const UWorld* World = GetWorld();
    if (!World) return nullptr;

    const AVbGameMode* GameMode = Cast<AVbGameMode>(World->GetAuthGameMode());

    return GameMode ? GameMode->GetDamageEffectClass() : nullptr;
}

void UVbCombatComponent::SendBlockedHitEvent(AActor* HitActor) const
{
    AVbCharacter* Character = Cast<AVbCharacter>(HitActor);
    if (!Character) return;

    Character->Client_SendGameplayEvent(Character, VbGameplayTags::Event_Blocked_Hit, FGameplayEventData());
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, VbGameplayTags::Event_Blocked_Hit, FGameplayEventData());
}

void UVbCombatComponent::CollectUniqueEnemyHits(const TArray<FHitResult>& HitResults, const IGenericTeamAgentInterface* OwnerTeamAgent,
    TArray<FHitResult>& OutHitResults, TSet<AActor*>* AlreadyHitActors) const
{
    if (!OwnerTeamAgent) return;

    TSet<AActor*> UniqueActorsThisStep;

    for (const FHitResult& HitResult : HitResults)
    {
        if (!HitResult.bBlockingHit) continue;

        AActor* HitActor = HitResult.GetActor();
        if (!HitActor) continue;

        const IGenericTeamAgentInterface* HitTeamAgent = Cast<IGenericTeamAgentInterface>(HitActor->GetInstigatorController());
        if (!HitTeamAgent || OwnerTeamAgent->GetGenericTeamId() == HitTeamAgent->GetGenericTeamId()) continue;

        if (UniqueActorsThisStep.Contains(HitActor)) continue;

        if (AlreadyHitActors && AlreadyHitActors->Contains(HitActor)) continue;
        if (AlreadyHitActors)
        {
            AlreadyHitActors->Add(HitActor);
        }

        UniqueActorsThisStep.Add(HitActor);
        OutHitResults.Add(HitResult);
    }
}

void UVbCombatComponent::StartAreaTrace(const FVbAreaTraceInfo& InAreaTraceInfo)
{
    AreaTraceInfo = InAreaTraceInfo;
    AreaTraceInfo.TraceIndex = 0;

    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(AreaTraceInfo.TimerHandle, this, &UVbCombatComponent::StepAreaTrace, AreaTraceInfo.HitInterval, true);
    }
}

void UVbCombatComponent::StepAreaTrace()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    if (AreaTraceInfo.TraceIndex >= AreaTraceInfo.TraceCount)
    {
        World->GetTimerManager().ClearTimer(AreaTraceInfo.TimerHandle);
        return;
    }

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor) return;

    const FVector TraceStart = AreaTraceInfo.bFollowOwner ? OwnerActor->GetActorLocation() : AreaTraceInfo.TraceStart;
    const FVector TraceEnd = AreaTraceInfo.bFollowOwner ? OwnerActor->GetActorLocation() : AreaTraceInfo.TraceEnd;

    const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECC_MeleeTrace);

    const IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(OwnerActor->GetInstigatorController());

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerActor);

    const EDrawDebugTrace::Type DrawDebug = AreaTraceInfo.bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    TArray<FHitResult> TraceResults;
    UKismetSystemLibrary::SphereTraceMulti(World, TraceStart, TraceEnd, AreaTraceInfo.Radius, TraceTypeQuery, false, ActorsToIgnore, DrawDebug, TraceResults,
        false);

    TSet<AActor*> AlreadyHitActors;

    TArray<FHitResult> OutHitResults;
    CollectUniqueEnemyHits(TraceResults, OwnerTeamAgent, OutHitResults, &AlreadyHitActors);

    for (const FHitResult& Result : OutHitResults)
    {
        OnAttackHitDetected.Broadcast(Result, AreaTraceInfo.AttackContext);
    }

    AreaTraceInfo.TraceIndex++;
}

void UVbCombatComponent::ApplyEffectOnHit(const FHitResult& HitResult, const FVbAttackContext& AttackContext) const
{
    AActor* Instigator = GetOwner();
    if (!Instigator || !Instigator->HasAuthority()) return;

    AActor* HitActor = HitResult.GetActor();
    if (!HitActor) return;

    const TSubclassOf<UGameplayEffect>& DamageEffectClass = GetDamageEffectClass();
    if (!DamageEffectClass) return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
    const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Instigator);
    if (!TargetASC || !SourceASC) return;

    if (ShouldIgnoreHit(SourceASC, TargetASC)) return;

    float Damage = AttackContext.Damage;

    bool bWasBlocked = false;
    if (!CanApplyDamageAfterDefense(HitActor, SourceASC, TargetASC, Instigator->GetActorLocation(), Damage, bWasBlocked)) return;

    FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
    ContextHandle.AddInstigator(Instigator, Instigator);
    ContextHandle.AddHitResult(HitResult);

    const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ContextHandle);
    SpecHandle.Data->SetByCallerTagMagnitudes.Add(VbGameplayTags::Data_Damage, Damage);
    SpecHandle.Data->AddDynamicAssetTag(AttackContext.ImpactTypeTag);
    SpecHandle.Data->AddDynamicAssetTag(AttackContext.StyleTag);

    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    if (!bWasBlocked && AttackContext.OnHitEventTag.IsValid())
    {
        FGameplayEventData EventData;
        EventData.Instigator = Instigator;
        EventData.Target = HitActor;
        EventData.ContextHandle.AddHitResult(HitResult);
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Instigator, AttackContext.OnHitEventTag, EventData);
    }
}