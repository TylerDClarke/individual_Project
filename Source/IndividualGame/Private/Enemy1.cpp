// Fill out your copyright notice in the Description page of Project Settings.

#include "IndividualGame.h"
#include "Enemy1.h"


AEnemy1::AEnemy1(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	EnemyMesh1 = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, "enemy1");
}


