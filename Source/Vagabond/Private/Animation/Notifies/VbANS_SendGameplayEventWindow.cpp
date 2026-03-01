// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Animation/Notifies/VbANS_SendGameplayEventWindow.h"

#include "AbilitySystemBlueprintLibrary.h"

FString UVbANS_SendGameplayEventWindow::GetNotifyName_Implementation() const
{
    FString StartTagStr = "None";
    FString EndTagStr = "None";
    if (StartTag.IsValid())
    {
        StartTagStr = StartTag.ToString();
    }

    if (EndTag.IsValid())
    {
        EndTagStr = EndTag.ToString();
    }

    return FString::Printf(TEXT("Send Window: %s | %s"), *StartTagStr, *EndTagStr);
}

void UVbANS_SendGameplayEventWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp || !MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), StartTag, FGameplayEventData());
}

void UVbANS_SendGameplayEventWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EndTag, FGameplayEventData());
}