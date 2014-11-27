// Fill out your copyright notice in the Description page of Project Settings.

#include "IndividualGame.h"
#include "DossierPickup.h"


ADossierPickup::ADossierPickup(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	
}

void ADossierPickup::onPickedUp_Implementation()
{
	//call parent implementation of onPickedUp
	Super::onPickedUp_Implementation();
	//Destroy the dossier
	Destroy();
}
