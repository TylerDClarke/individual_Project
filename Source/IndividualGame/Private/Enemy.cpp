// Fill out your copyright notice in the Description page of Project Settings.

#include "IndividualGame.h"
#include "Enemy.h"


AEnemy::AEnemy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	EnemyMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, "enemy"); 
	RootComponent = EnemyMesh;
}


