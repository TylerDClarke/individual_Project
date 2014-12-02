// Fill out your copyright notice in the Description page of Project Settings.

#include "IndividualGame.h"
#include "Pickup.h"


APickup::APickup(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	//The pickup is valid when it is created
	bIsActive = true;

	//create the root SphereComponent to handle pickups collision
	BaseCollisionComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponennt"));

	//set the SphereComponent as the root componrnt
	RootComponent = BaseCollisionComponent;

	//create the static mesh component
	PickupMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PickupMesh"));

	//Attach the StaticMesh Component to the root Conponent
	PickupMesh->AttachTo(RootComponent);
}

void APickup::onPickedUp_Implementation()
{
	//There is no default behaviour
}
