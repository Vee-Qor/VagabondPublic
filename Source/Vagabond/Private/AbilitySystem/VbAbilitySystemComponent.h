// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "VbAbilitySystemComponent.generated.h"

class UVbCharacterData;
class UVbGameplayAbility;

DECLARE_MULTICAST_DELEGATE(FOnAbilitiesChangedDelegate)

UCLASS()
class UVbAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UVbAbilitySystemComponent();

    FOnAbilitiesChangedDelegate OnAbilitiesChanged;

    void InitializeDefaultAttributes(const UVbCharacterData* CharacterData);
    void ApplyEffects(const TArray<TSubclassOf<UGameplayEffect>>& Effects);
    void ApplyEffect(const TSubclassOf<UGameplayEffect>& Effect);
    TArray<FGameplayAbilitySpecHandle> AddAbilities(const TArray<TSubclassOf<UVbGameplayAbility>>& AbilitiesToGive);
    void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove);

    void HandleInputPressed(const FGameplayTag& InputTag);
    void HandleInputReleased(const FGameplayTag& InputTag);

private:
    virtual void OnRep_ActivateAbilities() override;
    void BroadcastAbilitiesChanged() const;
    void UpdateExperience(const FOnAttributeChangeData& ChangeData);

    TArray<FGameplayAbilitySpec> OldActivatableAbilities;
};