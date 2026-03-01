// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/VbStyleData.h"
#include "VbPlayerStyleData.generated.h"

USTRUCT(BlueprintType)
struct FVbDodgeMontages
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* Forward = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* Backward = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* Left = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* Right = nullptr;
};

UCLASS()
class UVbPlayerStyleData : public UVbStyleData
{
    GENERATED_BODY()

public:
    FORCEINLINE const FVbDodgeMontages& GetDodgeMontages() const { return DodgeMontages; }
    FORCEINLINE UAnimMontage* GetEquipMontage() const { return EquipMontage; }
    FORCEINLINE UAnimMontage* GetUnequipMontage() const { return UnequipMontage; }

private:
    UPROPERTY(EditDefaultsOnly, Category="Combat|Animation")
    FVbDodgeMontages DodgeMontages;

    UPROPERTY(EditDefaultsOnly, Category="Combat|Animation")
    UAnimMontage* EquipMontage;

    UPROPERTY(EditDefaultsOnly, Category="Combat|Animation")
    UAnimMontage* UnequipMontage;
};