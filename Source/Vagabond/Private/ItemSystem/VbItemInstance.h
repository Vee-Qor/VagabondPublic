// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VbItemStatic.h"
#include "UObject/Object.h"
#include "VbItemInstance.generated.h"

class UVbItemStatic;

UCLASS()
class UVbItemInstance : public UObject
{
    GENERATED_BODY()

public:
    FORCEINLINE virtual bool IsSupportedForNetworking() const override { return true; }
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void Init(const UVbItemStatic* InItemStatic, int32 InQuantity);

    UPROPERTY(Replicated)
    const UVbItemStatic* ItemStatic;

    UPROPERTY(Replicated)
    int32 Quantity = 1;
};