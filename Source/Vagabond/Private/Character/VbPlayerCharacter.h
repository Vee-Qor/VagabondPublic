// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/VbCharacter.h"
#include "VbPlayerCharacter.generated.h"

class AVbGameplayHUD;
class UCameraComponent;
class UVbInputConfig;
class USpringArmComponent;
struct FGameplayTag;
struct FInputActionValue;

UCLASS()
class AVbPlayerCharacter : public AVbCharacter
{
    GENERATED_BODY()

public:
    AVbPlayerCharacter();
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PawnClientRestart() override;

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
    virtual void InitializeAbilityActorInfo() override;
    virtual void OnDying(const FGameplayTag Tag, int32 NewCount) override;
    virtual void OnRespawning(const FGameplayTag Tag, int32 NewCount) override;
    virtual void OnKnockdown(const FGameplayTag Tag, int32 NewCount) override;

private:
    void InitializeHUD();

    // ==== Input ==== //
    void HandleInputLook(const FInputActionValue& InputActionValue);
    void HandleInputMove(const FInputActionValue& InputActionValue);
    void HandleInputInteraction();
    void HandleInputInventoryOpen();
    void HandleInputEquipmentOpen();
    void HandleGameMenu();
    void HandleInputPressed(const FGameplayTag InputTag);
    void HandleInputReleased(const FGameplayTag InputTag);

    // ==== Utils ==== //
    AVbGameplayHUD* GetGameplayHUD() const;

    // ==== Camera ==== //
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
    UCameraComponent* FollowCamera;

    // ==== Runtime ==== //
    UPROPERTY()
    const UVbInputConfig* InputConfig;

    UPROPERTY()
    AVbGameplayHUD* GameplayHUD;
};