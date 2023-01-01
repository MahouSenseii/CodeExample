// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodeExampleCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ACodeExampleCharacter

ACodeExampleCharacter::ACodeExampleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ACodeExampleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACodeExampleCharacter::Tick(float DeltaTime)
{
	if (interactionList.Num() > 0)
	{
		UdateInteraction();
		bDoOnce = true;
	}
	else
	{
		if (bDoOnce == true)
		{
			UdateInteraction();
			bDoOnce = false;
		}
	}
}

void ACodeExampleCharacter::SetBestInteraction(AInteractionParent* Reference)
{
	AInteractionParent* newInteraction = Reference;
	if (newInteraction)
	{
		if (newInteraction != selectedInteraction)
		{
			selectedInteraction = newInteraction;
			selectedInteraction->ShowInteractionWidget();
		}
		else
		{
			selectedInteraction->HideInteractionWidget();
		}
	}
	else
	{
		if (selectedInteraction)
		{
			selectedInteraction->HideInteractionWidget();
			selectedInteraction = nullptr;
		}
	}
}

bool ACodeExampleCharacter::ShouldUpdateInteraction()
{
	bool update;
	/* limitation on when to search for interactions */
	if ((interactionList.Num() > 0) && (GetCharacterMovement()->MovementMode != MOVE_Falling))
	{
		update = true;
		return update;
	}
	else
	{
		update = false;
		return update;
	}
}

void ACodeExampleCharacter::UdateInteraction()
{
	bool results = ShouldUpdateInteraction();
	if (results == true)
	{
		SetBestInteraction(interactionReference);
		GetBestInteraction();
	}
	else
	{
		SetBestInteraction(interactionReference);
	}
}

void ACodeExampleCharacter::GetBestInteraction()
{
	TArray<float> dots;
	for (int i = 0; i < interactionList.Num(); i++)
	{
		FVector result = interactionList[i]->GetActorLocation() - this->GetActorLocation();
		result = result.GetSafeNormal(0.0001);
		FVector result2 = FollowCamera->GetForwardVector();
		float fResult = FVector::DotProduct(result, result2);
		dots.Add(fResult);
	}
	float maxValue = 0;
	int maxIndexValue = 0;
	int a;
	for (a = 0; a < dots.Num(); a++)
	{
		if (dots[a] > maxValue)
		{
			maxValue = dots[a];
			maxIndexValue = a;
		}
	}

	if (dots[maxIndexValue] > interactThreshold)
	{
		SetBestInteraction(interactionList[maxIndexValue]);

	}
	else
	{
		SetBestInteraction(interactionReference);
	}
}

AInteractionParent* ACodeExampleCharacter::AddInteraction(AInteractionParent* InteractableObject)
{
	interactionList.AddUnique(InteractableObject);

	return InteractableObject;
}

AInteractionParent* ACodeExampleCharacter::RemoveInteraction(AInteractionParent* InteractableObject)
{
	interactionList.Remove(InteractableObject);

	return InteractableObject;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACodeExampleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACodeExampleCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACodeExampleCharacter::Look);

	}

}

void ACodeExampleCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACodeExampleCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




