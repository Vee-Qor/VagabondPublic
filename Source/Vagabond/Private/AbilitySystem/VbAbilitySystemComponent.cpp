// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "AbilitySystem/VbAbilitySystemComponent.h"

#include "VbAttributeSet.h"
#include "Abilities/VbGameplayAbility.h"
#include "Data/VbCharacterData.h"
#include "Data/VbCharacterStats.h"
#include "Framework/VbGameInstance.h"

UVbAbilitySystemComponent::UVbAbilitySystemComponent()
{
    GetGameplayAttributeValueChangeDelegate(UVbAttributeSet::GetExperienceAttribute()).AddUObject(this, &UVbAbilitySystemComponent::UpdateExperience);
    UpdateExperience(FOnAttributeChangeData());
}

void UVbAbilitySystemComponent::InitializeDefaultAttributes(const UVbCharacterData* CharacterData)
{
    if (!CharacterData) return;

    const UVbCharacterStats* Stats = CharacterData->GetStats();
    if (!Stats) return;

    const FVbDefaultStatsValue& DefaultStats = Stats->GetDefault();

    SetNumericAttributeBase(UVbAttributeSet::GetMaxHealthAttribute(), DefaultStats.MaxHealth);
    SetNumericAttributeBase(UVbAttributeSet::GetMaxResourceAttribute(), DefaultStats.MaxResource);
    SetNumericAttributeBase(UVbAttributeSet::GetMaxPoiseAttribute(), DefaultStats.MaxPoise);
    SetNumericAttributeBase(UVbAttributeSet::GetAttackPowerAttribute(), DefaultStats.AttackPower);
    SetNumericAttributeBase(UVbAttributeSet::GetCriticalChanceAttribute(), DefaultStats.CriticalChance);
    SetNumericAttributeBase(UVbAttributeSet::GetCriticalMultiplierAttribute(), DefaultStats.CriticalMultiplier);
    SetNumericAttributeBase(UVbAttributeSet::GetArmorAttribute(), DefaultStats.Armor);

    const UWorld* World = GetWorld();
    if (!World) return;

    const UVbGameInstance* GameInstance = Cast<UVbGameInstance>(World->GetGameInstance());
    if (!GameInstance) return;

    const FRealCurve* ExperienceCurve = GameInstance->GetExperienceCurveTable();
    if (ExperienceCurve)
    {
        const float MaxLevel = ExperienceCurve->GetNumKeys();
        SetNumericAttributeBase(UVbAttributeSet::GetMaxLevelAttribute(), MaxLevel);

        const float MaxExperience = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
        SetNumericAttributeBase(UVbAttributeSet::GetMaxExperienceAttribute(), MaxExperience);
    }

    UpdateExperience(FOnAttributeChangeData());
}

void UVbAbilitySystemComponent::ApplyEffects(const TArray<TSubclassOf<UGameplayEffect>>& Effects)
{
    if (Effects.IsEmpty()) return;

    for (const TSubclassOf<UGameplayEffect>& Effect : Effects)
    {
        ApplyEffect(Effect);
    }
}

void UVbAbilitySystemComponent::ApplyEffect(const TSubclassOf<UGameplayEffect>& Effect)
{
    if (!Effect) return;

    const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Effect, 1.0f, ContextHandle);
    ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<FGameplayAbilitySpecHandle> UVbAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UVbGameplayAbility>>& AbilitiesToGive)
{
    TArray<FGameplayAbilitySpecHandle> NewAbilities;

    if (AbilitiesToGive.IsEmpty()) return NewAbilities;

    for (const TSubclassOf<UVbGameplayAbility>& AbilityClass : AbilitiesToGive)
    {
        if (!AbilityClass) continue;

        FGameplayAbilitySpec AbilitySpec(AbilityClass);

        if (const UVbGameplayAbility* VbAbility = Cast<UVbGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.GetDynamicSpecSourceTags().AddTag(VbAbility->GetInputTag());
        }

        NewAbilities.Add(GiveAbility(AbilitySpec));
    }

    BroadcastAbilitiesChanged();

    return NewAbilities;
}

void UVbAbilitySystemComponent::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove)
{
    if (AbilitiesToRemove.IsEmpty()) return;

    for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToRemove)
    {
        if (!SpecHandle.IsValid()) continue;

        ClearAbility(SpecHandle);
    }

    BroadcastAbilitiesChanged();
}

void UVbAbilitySystemComponent::OnRep_ActivateAbilities()
{
    Super::OnRep_ActivateAbilities();

    const TArray<FGameplayAbilitySpec>& CurrentAbilities = GetActivatableAbilities();
    bool bAbilitiesChanged = false;

    if (OldActivatableAbilities.Num() != CurrentAbilities.Num())
    {
        bAbilitiesChanged = true;
    }
    else
    {
        for (const FGameplayAbilitySpec& CurrentSpec : CurrentAbilities)
        {
            bool bFound = false;
            for (const FGameplayAbilitySpec& OldSpec : OldActivatableAbilities)
            {
                if (CurrentSpec.Handle == OldSpec.Handle)
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                bAbilitiesChanged = true;
                break;
            }
        }
    }

    if (!bAbilitiesChanged) return;

    BroadcastAbilitiesChanged();
    OldActivatableAbilities = CurrentAbilities;
}

void UVbAbilitySystemComponent::BroadcastAbilitiesChanged() const
{
    if (const UWorld* World = GetWorld())
    {
        const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([this]()
        {
            OnAbilitiesChanged.Broadcast();
        });

        World->GetTimerManager().SetTimerForNextTick(TimerDelegate);
    }
}

void UVbAbilitySystemComponent::UpdateExperience(const FOnAttributeChangeData& ChangeData)
{
    if (GetOwnerRole() != ROLE_Authority) return;

    bool bFound;
    const float CurrentLevel = GetGameplayAttributeValue(UVbAttributeSet::GetLevelAttribute(), bFound);
    const float MaxLevel = GetGameplayAttributeValue(UVbAttributeSet::GetMaxLevelAttribute(), bFound);
    if (CurrentLevel >= MaxLevel) return;

    const UWorld* World = GetWorld();
    if (!World) return;

    const UVbGameInstance* GameInstance = Cast<UVbGameInstance>(World->GetGameInstance());
    if (!GameInstance) return;

    const FRealCurve* ExperienceCurve = GameInstance->GetExperienceCurveTable();
    if (!ExperienceCurve) return;

    float PrevExperience = 0;
    float NextExperience = 0;
    float NewLevel = 1;

    const float CurrentExp = ChangeData.NewValue;

    for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
    {
        const float ExperienceToLevel = ExperienceCurve->GetKeyValue(*Iter);
        if (CurrentExp < ExperienceToLevel)
        {
            NextExperience = ExperienceToLevel;
            break;
        }
        PrevExperience = ExperienceToLevel;
        NewLevel = Iter.GetIndex() + 1;
    }

    SetNumericAttributeBase(UVbAttributeSet::GetLevelAttribute(), NewLevel);
    SetNumericAttributeBase(UVbAttributeSet::GetPrevExperienceAttribute(), PrevExperience);
    SetNumericAttributeBase(UVbAttributeSet::GetNextExperienceAttribute(), NextExperience);
}

void UVbAbilitySystemComponent::HandleInputPressed(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    ABILITYLIST_SCOPE_LOCK();

    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

        if (!AbilitySpec.IsActive())
        {
            TryActivateAbility(AbilitySpec.Handle);
        }
        else
        {
            const UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance();
            if (!AbilityInstance) continue;

            InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle,
                AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
        }
    }
}

void UVbAbilitySystemComponent::HandleInputReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;

    ABILITYLIST_SCOPE_LOCK();

    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

        const UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance();
        if (!AbilityInstance) continue;

        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle,
            AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
    }
}