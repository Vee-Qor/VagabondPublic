// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/VbCharacterData.h"
#include "VbPlayerData.generated.h"

UCLASS()
class UVbPlayerData : public UVbCharacterData
{
    GENERATED_BODY()
    
public:
    FORCEINLINE const UVbInputConfig* GetInputConfig() const { return InputConfig; }
    FORCEINLINE int32 GetDefaultMaxSlots() const { return DefaultMaxSlots; }
    FORCEINLINE const TMap<TSubclassOf<UVbItemStatic>, int32>& GetDefaultItemsMap() const { return DefaultItemsMap; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UVbInputConfig* InputConfig;

    UPROPERTY(EditDefaultsOnly, Category="Inventory")
    TMap<TSubclassOf<UVbItemStatic>, int32> DefaultItemsMap;

    UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(ClampMin="1"))
    int32 DefaultMaxSlots = 36;
};