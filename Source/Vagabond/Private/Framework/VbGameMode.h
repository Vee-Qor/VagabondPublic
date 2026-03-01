// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VbGameMode.generated.h"

class UGameplayEffect;

UCLASS()
class AVbGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AVbGameMode();

    FORCEINLINE const TSubclassOf<UGameplayEffect>& GetArmedStatusEffectClass() const { return ArmedStatusEffectClass; }
    FORCEINLINE const TSubclassOf<UGameplayEffect>& GetUnarmedStatusEffectClass() const { return UnarmedStatusEffectClass; }
    FORCEINLINE const TSubclassOf<UGameplayEffect>& GetDeadEffectClass() const { return DeadEffectClass; }
    FORCEINLINE const TSubclassOf<UGameplayEffect>& GetDamageEffectClass() const { return DamageEffectClass; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Effects", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UGameplayEffect> ArmedStatusEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Effects", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UGameplayEffect> UnarmedStatusEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Effects", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Effects", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UGameplayEffect> DeadEffectClass;
};