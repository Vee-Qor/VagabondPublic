// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Input/VbInputConfig.h"

const UInputAction* UVbInputConfig::FindInputActionByTag(const FGameplayTag& InputTag) const
{
    if (!InputTag.IsValid()) return nullptr;

    for (const FVbInputMappingConfig& Mapping : NativeInputMappings)
    {
        if (!Mapping.IsValid() || InputTag != Mapping.InputTag) continue;

        return Mapping.InputAction;
    }

    return nullptr;
}