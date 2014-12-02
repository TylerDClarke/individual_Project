// Fill out your copyright notice in the Description page of Project Settings.

#include "IndividualGame.h"
#include "Weapons.h"

#include "Engine.h"

#define TRACE_WEAPON ECC_GameTraceChannel1

AWeapons::AWeapons(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	CollisionComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollsionComponent"));
	RootComponent = CollisionComponent;

	WeaponMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh"));
	WeaponMesh->AttachTo(RootComponent);
}

void AWeapons::Fire()
{
	if (ProjectileType == EWeaponProjectile::EBullet)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Bullet"));
		InstantFire();
	}
	if (ProjectileType == EWeaponProjectile::ESpread)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Spread"));
		//this is going to have a random spread evrytime its called which is why it is placed in a for loop
		
		for (int32 i = 0; i <= WeaponConfig.bulletSpread; i++)
		{
			InstantFire();
		}
	}
	if (ProjectileType == EWeaponProjectile::EPRojectile)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Projectile"));
	}
}

void AWeapons::InstantFire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponConfig.bulletSpread;
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.bulletSpread * 0.5);
	const FVector AimDirection = WeaponMesh->GetSocketRotation("MF").Vector();
	const FVector StartTrace = WeaponMesh->GetSocketLocation("MF");
	const FVector ShootDirection = WeaponRandomStream.VRandCone(AimDirection, SpreadCone, SpreadCone);
	const FVector EndTrace = StartTrace + ShootDirection * WeaponConfig.shotRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDirection, RandomSeed, CurrentSpread);
}

FHitResult AWeapons::WeaponTrace(const FVector & traceFrom, const FVector &TraceTo) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit(ForceInit);

	GetWorld()->LineTraceSingle(Hit, traceFrom, TraceTo, TRACE_WEAPON, TraceParams);

	return Hit;
}

void AWeapons::ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &shootDirection, int32 RandomSeed, float ReticleSpread)
{
	const FVector EndTrace = Origin + shootDirection * WeaponConfig.shotRange;
	//if we hit an actor we want to end the trace otherwise we want to continue along the end trace till the end
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Black, true, 1000, 10.0f);

	AEnemy* Enemy = Cast<AEnemy>(Impact.GetActor());
	
	if (Enemy)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "You Hit A Target");
		Enemy->Destroy();
	}

	AEnemy1* Enemy1 = Cast<AEnemy1>(Impact.GetActor());

	if (Enemy1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "You Hit A Target");
		Enemy1->Destroy();
	}
}

