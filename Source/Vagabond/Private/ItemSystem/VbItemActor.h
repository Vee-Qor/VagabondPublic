// Copyright 2025 Vee.Qor. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VbItemActor.generated.h"

class UVbItemStatic;
class UVbItemInstance;
class USphereComponent;

UCLASS()
class AVbItemActor : public AActor
{
    GENERATED_BODY()

public:
    AVbItemActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    FORCEINLINE USphereComponent* GetPickupCollision() const { return PickupCollision; }
    FORCEINLINE UVbItemInstance* GetItemInstance() const { return ItemInstance; }
    FORCEINLINE TSubclassOf<UVbItemStatic> GetItemStatic() const { return ItemStatic; }

    FORCEINLINE void Init(UVbItemInstance* InItemInstance) { ItemInstance = InItemInstance; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleDefaultsOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleDefaultsOnly)
    USphereComponent* PickupCollision;

    UFUNCTION()
    void OnPickupCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPickupCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
    TSubclassOf<UVbItemStatic> ItemStatic;

    UPROPERTY(Replicated)
    UVbItemInstance* ItemInstance;

    UPROPERTY(EditAnywhere)
    int32 QuantityOnSpawn = 1;
};