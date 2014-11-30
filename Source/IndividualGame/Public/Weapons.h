// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "Weapons.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
namespace EWeaponProjectile
{
	enum ProjectileType
	{
		EBullet			UMETA(DisplayName = "Bullet"),
		ESpread			UMETA(DisplayName = "Spread"),
		EPRojectile		UMETA(DisplayName = "Projectile"),
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	//setting max amount of Ammo a weapon can hace
	UPROPERTY(EditDefaultsOnly, Category = Ammunition)
	int32 MaxAmmunition;

	//Makes it so that shotguns and pistols dont rapid firee
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float TimeBetweenShots;

	//check what kind of bullet shot there is whether it shoots one bullet or burst of three
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 ShotCost;

	//checks the range so a shotgun wont fire and hit ages away
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float shotRange;

	//does it fire straight or does it have a spread like shotgun
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float bulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	FString Name;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ADistractObject> DistractObject;

	
};


UCLASS()
class INDIVIDUALGAME_API AWeapons : public AActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION()
	void Fire();

	UFUNCTION()
	void InstantFire();
	
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, BLueprintReadWrite, Category = Config)
	TEnumAsByte<EWeaponProjectile::ProjectileType> ProjectileType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	TSubobjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
	TSubobjectPtr<USkeletalMeshComponent> WeaponMesh;

protected:
	FHitResult WeaponTrace(const FVector & traceFrom, const FVector &TraceTo) const;
	
	void ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &shootDirection, int32 RandomSeed, float ReticleSpread);

};
