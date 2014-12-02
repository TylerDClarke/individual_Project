// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "IndividualGame.h"
#include "IndividualGameCharacter.h"
#include "Engine.h"
#include "Engine/Blueprint.h"

//////////////////////////////////////////////////////////////////////////
// AIndividualGameCharacter

AIndividualGameCharacter::AIndividualGameCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Inventory.SetNum(2, false);

	//CurrentWeapon = NULL;

	// Set size for collision capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	CharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	CharacterMovement->JumpZVelocity = 600.f;
	CharacterMovement->AirControl = 0.2f;

	CollisionComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CollisionComponent"));
	CollisionComponent->AttachTo(RootComponent);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AIndividualGameCharacter::OnCollision);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AIndividualGameCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &AIndividualGameCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AIndividualGameCharacter::MoveRight);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AIndividualGameCharacter::FireWeapon);

	InputComponent->BindAction("Pistol", IE_Pressed, this, &AIndividualGameCharacter::EquipPistol);
	InputComponent->BindAction("Shotgun", IE_Pressed, this, &AIndividualGameCharacter::EquipShotgun);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AIndividualGameCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AIndividualGameCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AIndividualGameCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AIndividualGameCharacter::TouchStopped);
}

void AIndividualGameCharacter::BeginPlay()
{
	//FActorSpawnParameters SpawnParameters;
	//SpawnParameters.Owner = this;
	//SpawnParameters.Instigator = Instigator;
	//AWeapons* Spawner = GetWorld()->SpawnActor<AWeapons>(WeaponSpawn, SpawnParameters);

	//if (Spawner)
	//{
	//	Spawner->AttachRootComponentTo(Mesh, "Weapon_Socket", EAttachLocation::SnapToTarget);
	//	CurrentWeapon = Spawner;
	//}
}


void AIndividualGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AIndividualGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void AIndividualGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AIndividualGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AIndividualGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AIndividualGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void AIndividualGameCharacter::FireWeapon()
{
	if (CurrentWeapon != NULL){
		CurrentWeapon->Fire();
	}
}

void AIndividualGameCharacter::OnCollision(AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	APistol* Pistol = Cast<APistol>(OtherActor);
	if (Pistol){
		Inventory[0] = Pistol->GetClass();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "I Just Picked up a " + Pistol->WeaponConfig.Name);
		Pistol->Destroy();
	}
	AShotgun* Shotgun = Cast<AShotgun>(OtherActor);
	if (Shotgun){
		Inventory[1] = Shotgun->GetClass();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "I Just Picked up a " + Shotgun->WeaponConfig.Name);
		Shotgun->Destroy();
	}
}

void AIndividualGameCharacter::EquipPistol(){
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = Instigator;
	AWeapons* Spawner = GetWorld()->SpawnActor<AWeapons>(Inventory[0], SpawnParameters);

	if (Spawner)
	{
		if (CurrentWeapon != NULL){
			for (int32 i = 0; i < 2; i++){
				if (Inventory[i] != NULL && Inventory[i]->GetDefaultObject<AWeapons>()->WeaponConfig.Name == CurrentWeapon->WeaponConfig.Name){
					Inventory[i] = NULL;
					Inventory[i] = CurrentWeapon->GetClass();
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "I put" + CurrentWeapon->WeaponConfig.Name + "Away in slot" + FString::FromInt(i));
				}
			}
			CurrentWeapon->Destroy();
			Spawner->CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Spawner->AttachRootComponentTo(Mesh, "Weapon_Socket", EAttachLocation::SnapToTarget);
			CurrentWeapon = Spawner;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "My Current weapon is" + CurrentWeapon->WeaponConfig.Name);
		}
		else{
			Spawner->CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Spawner->AttachRootComponentTo(Mesh, "Weapon_Socket", EAttachLocation::SnapToTarget);
			CurrentWeapon = Spawner;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "My Current weapon is" + CurrentWeapon->WeaponConfig.Name);
		}
	}
}

void AIndividualGameCharacter::EquipShotgun(){
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = Instigator;
	AWeapons* Spawner = GetWorld()->SpawnActor<AWeapons>(Inventory[1], SpawnParameters);

	if (Spawner)
	{
		if (CurrentWeapon != NULL){
			for (int32 i = 0; i < 2; i++){
				if (Inventory[i] != NULL && Inventory[i]->GetDefaultObject<AWeapons>()->WeaponConfig.Name == CurrentWeapon->WeaponConfig.Name){
					Inventory[i] = NULL;
					Inventory[i] = CurrentWeapon->GetClass();
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "I put" + CurrentWeapon->WeaponConfig.Name + "Away in slot" + FString::FromInt(i));
				}
			}
			CurrentWeapon->Destroy();
			Spawner->CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Spawner->AttachRootComponentTo(Mesh, "Weapon_Socket", EAttachLocation::SnapToTarget);
			CurrentWeapon = Spawner;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "My Current weapon is" + CurrentWeapon->WeaponConfig.Name);
		}
		else{
			Spawner->CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Spawner->AttachRootComponentTo(Mesh, "Weapon_Socket", EAttachLocation::SnapToTarget);
			CurrentWeapon = Spawner;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, "My Current weapon is" + CurrentWeapon->WeaponConfig.Name);
		}
	}
}