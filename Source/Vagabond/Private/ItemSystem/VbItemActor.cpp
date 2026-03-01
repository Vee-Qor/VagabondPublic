// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "ItemSystem/VbItemActor.h"

#include "Components/SphereComponent.h"
#include "Character/VbInteractionComponent.h"
#include "Engine/ActorChannel.h"
#include "ItemSystem/VbItemInstance.h"
#include "Net/UnrealNetwork.h"

AVbItemActor::AVbItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicatingMovement(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
    Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Mesh->SetSimulatePhysics(true);
    SetRootComponent(Mesh);

    PickupCollision = CreateDefaultSubobject<USphereComponent>("InteractCollision");
    PickupCollision->SetupAttachment(Mesh);
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AVbItemActor::OnPickupCollisionBeginOverlap);
    PickupCollision->OnComponentEndOverlap.AddDynamic(this, &AVbItemActor::OnPickupCollisionEndOverlap);
}

void AVbItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AVbItemActor, ItemInstance);
}

bool AVbItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    if (ItemInstance)
    {
        bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
    }

    return bWroteSomething;
}

void AVbItemActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority() && !ItemInstance && ItemStatic)
    {
        ItemInstance = NewObject<UVbItemInstance>(this);
        ItemInstance->Init(ItemStatic.GetDefaultObject(), QuantityOnSpawn);
    }
}

void AVbItemActor::OnPickupCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    if (UVbInteractionComponent* InteractionComponent = OtherActor->FindComponentByClass<UVbInteractionComponent>())
    {
        InteractionComponent->AddPickupTarget(this);
    }
}

void AVbItemActor::OnPickupCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    if (UVbInteractionComponent* InteractionComponent = OtherActor->FindComponentByClass<UVbInteractionComponent>())
    {
        InteractionComponent->RemovePickupTarget(this);
    }
}