// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Animation/Notifies/VbAN_PlayFootstep.h"

FString UVbAN_PlayFootstep::GetNotifyName_Implementation() const
{
    const UEnum* FootEnum = StaticEnum<EVbFoot>();
    const UEnum* MoveEnum = StaticEnum<EVbFootstepMovement>();

    const int64 FootValue = static_cast<int64>(FootstepContext.Foot);
    const int64 MoveValue = static_cast<int64>(FootstepContext.FootstepMovement);

    const FText FootText = FootEnum ? FootEnum->GetDisplayNameTextByValue(FootValue) : FText::FromString("None");
    const FText MoveText = MoveEnum ? MoveEnum->GetDisplayNameTextByValue(MoveValue) : FText::FromString("None");

    return FString::Printf(TEXT("Footstep: %s | %s"), *FootText.ToString(), *MoveText.ToString());
}

void UVbAN_PlayFootstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (MeshComp)
    {
        const AActor* OwnerActor = MeshComp->GetOwner();
        if (!OwnerActor) return;

        const UVbFootstepComponent* FootstepComponent = OwnerActor->FindComponentByClass<UVbFootstepComponent>();
        if (!FootstepComponent) return;

        FootstepComponent->HandleFootStep(FootstepContext);
    }
}