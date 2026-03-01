// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Animation/Notifies/VbAN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

FString UVbAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
    FString TagStr = "None";
    if (EventTag.IsValid())
    {
        TagStr = EventTag.ToString();
    }

    return FString::Printf(TEXT("Send: %s"), *TagStr);
}

void UVbAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData{});
}