// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Weapons.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "DossierPickup.h"
#include "IndividualGameCharacter.generated.h"

UCLASS(config=Game)
class AIndividualGameCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class UCameraComponent> FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	TSubobjectPtr<class UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = Spawn)
	TSubclassOf<class AWeapons> WeaponSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collect)
	TSubobjectPtr<class USphereComponent> CollisionSphere;


protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	void FireWeapon();

	TArray<TSubclassOf<AWeapons>> Inventory;

	AWeapons *CurrentWeapon;

	UFUNCTION()
	virtual void BeginPlay() override;

	void EquipPistol();
	void EquipShotgun();

	UFUNCTION()
	void OnCollision(AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = Collect)
	void CollectDossier();

	void StartSprint();
	void EndSprint();

	UFUNCTION(BlueprintCallable, Category = Sprint)
	void sprintDrain();

	float SprintEnergy; 

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface
};

