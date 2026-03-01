// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Character/VbPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VbAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/VbCharacterData.h"
#include "EnhancedInputSubsystems.h"
#include "VbGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "VbInteractionComponent.h"
#include "Data/Player/VbPlayerData.h"
#include "Input/VbInputComponent.h"
#include "Input/VbInputConfig.h"
#include "ItemSystem/VbInventoryComponent.h"
#include "Player/VbPlayerState.h"
#include "UI/HUD/VbGameplayHUD.h"

AVbPlayerCharacter::AVbPlayerCharacter()
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCapsuleComponent()->SetCapsuleSize(30.0f, 90.0f);

    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxStepHeight = 50.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->TargetArmLength = 350.0f;
    CameraBoom->SocketOffset = FVector(0.0f, 25.0f, 75.0f);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;
}

void AVbPlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    InitializeAbilityActorInfo();
}

void AVbPlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    InitializeAbilityActorInfo();
}

void AVbPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UVbInputComponent* VbInputComponent = CastChecked<UVbInputComponent>(PlayerInputComponent);

    const UVbPlayerData* PlayerData = Cast<UVbPlayerData>(GetCharacterData());

    InputConfig = PlayerData ? PlayerData->GetInputConfig() : nullptr;
    if (!InputConfig) return;

    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &AVbPlayerCharacter::HandleInputLook);
    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &AVbPlayerCharacter::HandleInputMove);
    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_Interaction, ETriggerEvent::Started, this,
        &AVbPlayerCharacter::HandleInputInteraction);
    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_Inventory_Open, ETriggerEvent::Started, this,
        &AVbPlayerCharacter::HandleInputInventoryOpen);
    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_Equipment_Open, ETriggerEvent::Started, this,
        &AVbPlayerCharacter::HandleInputEquipmentOpen);
    VbInputComponent->BindNativeInputMapping(InputConfig, VbGameplayTags::Input_GameMenu, ETriggerEvent::Started, this, &AVbPlayerCharacter::HandleGameMenu);

    VbInputComponent->BindAbilityInputMapping(InputConfig, this, &AVbPlayerCharacter::HandleInputPressed, &AVbPlayerCharacter::HandleInputReleased);
}

void AVbPlayerCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();

    const APlayerController* PlayerController = GetController<APlayerController>();
    if (!PlayerController) return;

    const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(Subsystem);

    const UInputMappingContext* DefaultMappingContext = InputConfig ? InputConfig->GetDefaultMappingContext() : nullptr;
    if (!DefaultMappingContext) return;

    Subsystem->RemoveMappingContext(DefaultMappingContext);
    Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AVbPlayerCharacter::InitializeAbilityActorInfo()
{
    AVbPlayerState* VbPlayerState = GetPlayerState<AVbPlayerState>();
    if (!VbPlayerState) return;

    SetVbAbilitySystemComponent(VbPlayerState->GetVbAbilitySystemComponent());
    SetVbAttributeSet(VbPlayerState->GetVbAttributeSet());

    if (GetVbAbilitySystemComponent())
    {
        GetVbAbilitySystemComponent()->InitAbilityActorInfo(VbPlayerState, this);
        InitializeHUD();
    }

    if (GetInventoryComponent())
    {
        if (const UVbPlayerData* PlayerData = Cast<UVbPlayerData>(GetCharacterData()))
        {
            GetInventoryComponent()->Initialize(PlayerData->GetDefaultItemsMap(), PlayerData->GetDefaultMaxSlots());
        }
    }

    Super::InitializeAbilityActorInfo();
}

AVbGameplayHUD* AVbPlayerCharacter::GetGameplayHUD() const
{
    const APlayerController* PlayerController = Cast<APlayerController>(GetController());

    return PlayerController ? Cast<AVbGameplayHUD>(PlayerController->GetHUD()) : nullptr;
}

void AVbPlayerCharacter::OnDying(const FGameplayTag Tag, int32 NewCount)
{
    Super::OnDying(Tag, NewCount);

    if (NewCount > 0)
    {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (!PlayerController) return;

        PlayerController->SetIgnoreMoveInput(true);
        PlayerController->SetIgnoreLookInput(true);
    }
}

void AVbPlayerCharacter::OnRespawning(const FGameplayTag Tag, int32 NewCount)
{
    Super::OnRespawning(Tag, NewCount);

    if (NewCount > 0)
    {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (!PlayerController) return;

        PlayerController->SetIgnoreMoveInput(false);
        PlayerController->SetIgnoreLookInput(false);
    }
}

void AVbPlayerCharacter::OnKnockdown(const FGameplayTag Tag, int32 NewCount)
{
    Super::OnKnockdown(Tag, NewCount);

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) return;

    PlayerController->SetIgnoreMoveInput(NewCount > 0);
}

void AVbPlayerCharacter::InitializeHUD()
{
    GameplayHUD = GetGameplayHUD();
    if (!GameplayHUD) return;

    GameplayHUD->CreateHUDAndInitializeWithASC(GetVbAbilitySystemComponent());
}

void AVbPlayerCharacter::HandleInputLook(const FInputActionValue& InputActionValue)
{
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

    if (LookAxisVector.Y != 0.0f)
    {
        AddControllerPitchInput(LookAxisVector.Y);
    }

    if (LookAxisVector.X != 0.0f)
    {
        AddControllerYawInput(LookAxisVector.X);
    }
}

void AVbPlayerCharacter::HandleInputMove(const FInputActionValue& InputActionValue)
{
    const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);

    if (MovementVector.Y != 0.0f)
    {
        const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }

    if (MovementVector.X != 0.0f)
    {
        const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AVbPlayerCharacter::HandleInputPressed(const FGameplayTag InputTag)
{
    if (!GetVbAbilitySystemComponent()) return;

    if (InputTag == VbGameplayTags::Input_AbilityConfirm)
    {
        GetVbAbilitySystemComponent()->LocalInputConfirm();
        return;
    }

    if (InputTag == VbGameplayTags::Input_AbilityCancel)
    {
        GetVbAbilitySystemComponent()->LocalInputCancel();
        return;
    }

    GetVbAbilitySystemComponent()->HandleInputPressed(InputTag);
}

void AVbPlayerCharacter::HandleInputReleased(const FGameplayTag InputTag)
{
    if (!GetVbAbilitySystemComponent()) return;

    GetVbAbilitySystemComponent()->HandleInputReleased(InputTag);
}

void AVbPlayerCharacter::HandleInputInteraction()
{
    if (GetInteractionComponent())
    {
        GetInteractionComponent()->TryPickupItem();
    }
}

void AVbPlayerCharacter::HandleInputInventoryOpen()
{
    if (!GameplayHUD) return;

    GameplayHUD->ToggleInventory();
}

void AVbPlayerCharacter::HandleInputEquipmentOpen()
{
    if (!GameplayHUD) return;

    GameplayHUD->ToggleEquipment();
}

void AVbPlayerCharacter::HandleGameMenu()
{
    if (!GameplayHUD) return;

    GameplayHUD->ToggleGameMenu();
}