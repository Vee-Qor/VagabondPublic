// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/VbCharacter.h"
#include "VbEnemyCharacter.generated.h"

class UWidgetComponent;
class USphereComponent;

UCLASS()
class AVbEnemyCharacter : public AVbCharacter
{
    GENERATED_BODY()

public:
    AVbEnemyCharacter();
    virtual void PostInitializeComponents() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ==== Accessors ==== //
    FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }
    FORCEINLINE void SetTargetActor(AActor* InTargetActor) { TargetActor = InTargetActor; }

protected:
    virtual void BeginPlay() override;
    virtual void InitializeAbilityActorInfo() override;
    virtual void OnDying(const FGameplayTag Tag, int32 NewCount) override;
    virtual void OnStun(const FGameplayTag Tag, int32 NewCount) override;

private:
    void InitializeOverlay() const;

    // ==== Components ==== //
    UPROPERTY(VisibleDefaultsOnly, Category="Component")
    UWidgetComponent* EnemyOverlayWidgetComponent;

    // ==== Runtime ==== //
    UPROPERTY(Replicated)
    AActor* TargetActor;
};