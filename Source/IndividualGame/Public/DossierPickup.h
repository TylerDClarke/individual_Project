// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "DossierPickup.generated.h"

/**
*
*/
UCLASS()
class INDIVIDUALGAME_API ADossierPickup : public APickup
{
	GENERATED_UCLASS_BODY()

	/*doesnt need new UFUNCTION as it can call the old one
	This override the old onPickedUp_Implementation and is
	referred to as implementation because of the BlueprintNativeEvent*/
	void onPickedUp_Implementation() override;


};
