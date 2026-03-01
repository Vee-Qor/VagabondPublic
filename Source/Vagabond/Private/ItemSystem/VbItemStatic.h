// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "VbItemStatic.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class AVbItemActor;
class UVbStyleData;

UCLASS(BlueprintType, Blueprintable)
class UVbItemStatic : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UVbStyleData* StyleData;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Item"))
    FGameplayTag ItemTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Equipment.Slot"))
    FGameplayTagContainer AllowedEquipSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1))
    int32 MaxStackCount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AVbItemActor> ItemActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    bool IsEquippable() const { return !AllowedEquipSlots.IsEmpty(); }
};