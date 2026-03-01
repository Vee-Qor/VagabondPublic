// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/VbInputConfig.h"
#include "VbInputComponent.generated.h"

UCLASS()
class UVbInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    template <class UserObject, typename FuncType>
    void BindNativeInputMapping(const UVbInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserObject* Object,
        FuncType Func);

    template <class UserObject, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityInputMapping(const UVbInputConfig* InputConfig, UserObject* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template <class UserObject, typename FuncType>
void UVbInputComponent::BindNativeInputMapping(const UVbInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserObject* Object,
    FuncType Func)
{
    check(InputConfig);

    if (const UInputAction* FoundAction = InputConfig->FindInputActionByTag(InputTag))
    {
        BindAction(FoundAction, TriggerEvent, Object, Func);
    }
}

template <class UserObject, typename PressedFuncType, typename ReleasedFuncType>
void UVbInputComponent::BindAbilityInputMapping(const UVbInputConfig* InputConfig, UserObject* Object, PressedFuncType PressedFunc,
    ReleasedFuncType ReleasedFunc)
{
    check(InputConfig);

    for (const FVbInputMappingConfig& Config : InputConfig->GetAbilityInputMappings())
    {
        if (!Config.IsValid()) continue;

        BindAction(Config.InputAction, ETriggerEvent::Started, Object, PressedFunc, Config.InputTag);
        BindAction(Config.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Config.InputTag);
    }
}