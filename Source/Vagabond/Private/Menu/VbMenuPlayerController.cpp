// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Menu/VbMenuPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

AVbMenuPlayerController::AVbMenuPlayerController()
{
    bAutoManageActiveCameraTarget = false;
}

void AVbMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    SetInputMode(FInputModeGameAndUI());

    if (IsLocalPlayerController())
    {
        SetViewToMenuCamera();
    }
}

void AVbMenuPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (!PlayerState) return;

    OnPlayerStateReady.Broadcast(PlayerState);
}

void AVbMenuPlayerController::SetViewToMenuCamera()
{
    const UWorld* World = GetWorld();
    if (!World) return;

    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsOfClassWithTag(World, ACameraActor::StaticClass(), MenuCameraTag, Cameras);

    if (Cameras.Num() <= 0) return;

    ACameraActor* CameraActor = Cast<ACameraActor>(Cameras[0]);
    if (!CameraActor) return;

    SetViewTargetWithBlend(CameraActor);
}