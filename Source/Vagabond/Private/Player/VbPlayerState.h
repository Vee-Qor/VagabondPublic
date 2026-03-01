// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "VbPlayerState.generated.h"

class AVbExtractionZone;
class UVbAttributeSet;
class UVbAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnReadyChangedDelegate, bool /*bNewReady*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNameChangedDelegate, const FString& /*Name*/);

UCLASS()
class AVbPlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AVbPlayerState();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRep_PlayerName() override;

    FOnReadyChangedDelegate OnReadyChanged;
    FOnNameChangedDelegate OnNameChanged;

    // ==== Accessors ==== //
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintPure, Category="AbilitySystem")
    FORCEINLINE UVbAbilitySystemComponent* GetVbAbilitySystemComponent() const { return VbAbilitySystemComponent; };

    UFUNCTION(BlueprintPure, Category="AbilitySystem")
    FORCEINLINE UVbAttributeSet* GetVbAttributeSet() const { return VbAttributeSet; };

    FORCEINLINE bool IsReady() const { return bReady; }

    UFUNCTION(Server, Reliable)
    void Server_ToggleReady();

    void SetExtractionZone(AVbExtractionZone* InExtractionZone) { ExtractionZone = InExtractionZone; };
    void SetReady(bool bNewReady);

private:
    UPROPERTY(ReplicatedUsing=OnRep_ExtractionZone)
    AVbExtractionZone* ExtractionZone;

    UFUNCTION()
    void OnRep_ExtractionZone() const;

    UPROPERTY(ReplicatedUsing=OnRep_Ready)
    bool bReady = false;

    UFUNCTION()
    void OnRep_Ready() const;

    // ==== Ability System ==== //
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
    UVbAbilitySystemComponent* VbAbilitySystemComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
    UVbAttributeSet* VbAttributeSet;
};