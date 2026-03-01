// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "VbANS_SendGameplayEventWindow.generated.h"

UCLASS()
class UVbANS_SendGameplayEventWindow : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual FString GetNotifyName_Implementation() const override;
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
        const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category="Tags")
    FGameplayTag StartTag;

    UPROPERTY(EditAnywhere, Category="Tags")
    FGameplayTag EndTag;
};