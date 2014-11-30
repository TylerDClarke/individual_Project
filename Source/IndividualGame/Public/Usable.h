// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Usable.generated.h"

/**
 * Followed the tutorial on http://www.tomlooman.com/tutorial-usableactor-system-in-c/
 */
UCLASS()
class INDIVIDUALGAME_API AUsable : public AStaticMeshActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintImplementableEvent)
	bool onUsed(ACharacter* character);

	UFUNCTION(BlueprintImplementableEvent)
	bool StartItem();

	UFUNCTION(BlueprintImplementableEvent)
	bool EndItem();
};
