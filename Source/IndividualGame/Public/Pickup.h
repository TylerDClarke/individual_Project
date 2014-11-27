// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class INDIVIDUALGAME_API APickup : public AActor
{
	GENERATED_UCLASS_BODY()

	// True when the pickup is able to be picked up, false if the pickup isnt available
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Pickup)
	bool bIsActive; 

	//Simple collision primitive to use as the root component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
	TSubobjectPtr<USphereComponent> BaseCollisionComponent;

	//static mesh is the is to represent what is being picked up in the level 
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Pickup)
	TSubobjectPtr<UStaticMeshComponent> PickupMesh;

	//Function to call when our pickup is collected.
	UFUNCTION(BlueprintNativeEvent)
	void onPickedUp();
};
