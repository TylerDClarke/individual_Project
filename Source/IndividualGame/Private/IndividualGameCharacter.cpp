// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "IndividualGame.h"
#include "IndividualGameCharacter.h"
#include "DossierPickup.h"

//////////////////////////////////////////////////////////////////////////
// AIndividualGameCharacter

AIndividualGameCharacter::AIndividualGameCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
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

	//Create the dossierCollectVolume
	CollectionSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);

	maxUseDistance = 150;
	bHasNewFocus = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AIndividualGameCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AIndividualGameCharacter::startSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AIndividualGameCharacter::endSprint);

	InputComponent->BindAction("Use", IE_Pressed, this, &AIndividualGameCharacter::use);

	InputComponent->BindAxis("MoveForward", this, &AIndividualGameCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AIndividualGameCharacter::MoveRight);

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

	InputComponent->BindAction("CollectionPickups", IE_Pressed, this, &AIndividualGameCharacter::collectDossier);
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

void AIndividualGameCharacter::startSprint()
{
	CharacterMovement->MaxWalkSpeed = 900;
}

void AIndividualGameCharacter::endSprint()
{
	CharacterMovement->MaxWalkSpeed = 600;
}

void AIndividualGameCharacter::collectDossier()
{
	//get all overlapping Actors and store them in a collectedActor array
	TArray<AActor*> collectedActor;
	CollectionSphere->GetOverlappingActors(collectedActor);
	
	//for each actor collected
	for (int32 iCollected = 0; iCollected < collectedActor.Num(); ++iCollected)
	{
		//Cast the collected to ADossierPickUp
		ADossierPickup* const Dossier = Cast<ADossierPickup>(collectedActor[iCollected]);

		//if cast is successful, and battery is valid and active
		if (Dossier && !Dossier->IsPendingKill() && Dossier->bIsActive)
		{
			//call the Dossier pick up function
			Dossier->onPickedUp();
			//set isActive to false
			Dossier->bIsActive = false;
		}
	}
}


//Performs a ray trace to find closest looked at usbale actor
AUsable* AIndividualGameCharacter::getUsableView()
{
	FVector cameraLocation;
	FRotator cameraRotation;



	if (Controller == NULL)
		return NULL;

	//Get the location of the player and then cast a trace from where they are pointing by the distance cast in the constructor which is 150
	Controller->GetPlayerViewPoint(cameraLocation, cameraRotation);
	const FVector start_trace = cameraLocation;
	const FVector direction = cameraRotation.Vector();
	const FVector end_Trace = start_trace + (direction * maxUseDistance);

	//setting the parameters of the trace so not everything in the world is outlined
	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, start_trace, end_Trace, COLLISION_PROJECTILE, TraceParams);

	return Cast<AUsable>(Hit.GetActor());
}


//update actor currently being looked at by player
void AIndividualGameCharacter::Tick(float DeltaSeconds)
	{
	Super::Tick(DeltaSeconds);

	if (Controller && Controller->IsLocalController())
	{
		AUsable* usable = getUsableView();

		//EndFocus
		if(FocusedUsableActor != usable)
		{
			if(FocusedUsableActor)
			{
				FocusedUsableActor->EndItem();
			}
			bHasNewFocus = true;
		}	

		//Assign a new focus
		FocusedUsableActor = usable;

		//Start Focus
		if (usable)
		{
			if (bHasNewFocus)
			{
				usable->StartItem();
				bHasNewFocus = false;
			}
		}
	}
}

/*Runs on server. Onused is called if views actor is implemented*/
void AIndividualGameCharacter::use_Implementation()
{
	AUsable* usable = getUsableView();
	if (usable)
	{
		usable->onUsed(this);
	}
}

bool AIndividualGameCharacter::use_Validate()
{
	//No special validation performed
	return true;
}

