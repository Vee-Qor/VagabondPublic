// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "ItemSystem/VbItemInstance.h"

#include "Net/UnrealNetwork.h"

void UVbItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UVbItemInstance, ItemStatic);
    DOREPLIFETIME(UVbItemInstance, Quantity);
}

void UVbItemInstance::Init(const UVbItemStatic* InItemStatic, int32 InQuantity)
{
    ItemStatic = InItemStatic;
    Quantity = InQuantity;
}