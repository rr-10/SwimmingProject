// Copyright Epic Games, Inc. All Rights Reserved.

#include "SwimCharacter.h"

#include <string>

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

//////////////////////////////////////////////////////////////////////////
// SwimCharacter

ASwimCharacter::ASwimCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

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

	// Set timer for the swimming function
	//GetWorld()->GetTimerManager().SetTimer(SwimmingTimerHandle, this, &ASwimCharacter::Swimming, 1.0f, true);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	PrimaryActorTick.bCanEverTick = true;

	//USwimGameInstance GameInstance;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASwimCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	//PlayerInputComponent->BindAction("FastSwim", IE_Pressed, this, &ASwimCharacter::StartFastSwimming);
	//PlayerInputComponent->BindAction("FastSwim", IE_Released, this, &ASwimCharacter::EndFastSwimming);
	PlayerInputComponent->BindAction("FastSwim", IE_Pressed, this, &ASwimCharacter::StartFastSwim);
	PlayerInputComponent->BindAction("FastSwim", IE_Released, this, &ASwimCharacter::EndFastSwim);
	
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASwimCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASwimCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATP_ThirdPersonCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATP_ThirdPersonCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASwimCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASwimCharacter::TouchStopped);
}

void ASwimCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASwimCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ASwimCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASwimCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ASwimCharacter::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASwimCharacter::CountDown, 1.f, true, 0.0);
	// Regen of stamina
	//GetWorldTimerManager().SetTimer(StaminaHandle, this, &ASwimCharacter::DecreaseStamina, 1.0f, true);
	GetWorldTimerManager().SetTimer(StaminaHandle, this, &ASwimCharacter::RegenStamina, 1.0f, true);
	//Stamina = 100.0f;
	SwimmingSpeed = 150.0f;
	GetWorldTimerManager().SetTimer(SwimmingHandle, this, &ASwimCharacter::HandleFastSwim, 1.0f, true);
	
}

void ASwimCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		//FString ValueString = FString::SanitizeFloat(Value);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ValueString);
	}
}

/*void ASwimCharacter::StartFastSwimming()
{
	//float Speed = CharacterMovement->MaxSwimSpeed;
	GetCharacterMovement()->MaxSwimSpeed = 300;
	IsFastSwimming = true;
	DecreaseStamina();
	FString SpeedString = FString::SanitizeFloat(GetCharacterMovement()->MaxSwimSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FAST SWIM") + SpeedString);
}*/

/*void ASwimCharacter::EndFastSwimming()
{
	GetCharacterMovement()->MaxSwimSpeed = 150;
	IsFastSwimming = false;
	RecuperateStamina();
}*/

void ASwimCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
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

void ASwimCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GetCharacterMovement()->MaxSwimSpeed = 300;
	// float SwimSpeed = GetCharacterMovement()->BrakingDecelerationSwimming;
	// FString SwimString = FString::SanitizeFloat(SwimSpeed);
	//FString SwimString = FString::
	// GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, SwimString);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, UKismetStringLibrary::Conv_BoolToString(IsFastSwimming));

}

// Instead of tick, the function runs using the timer
void ASwimCharacter::Swimming()
{
	float DifferenceZ = WaterZ - GetActorLocation().Z;
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	bool WaterVolume = GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume;
	
	//bool SwimDoOnce, WalkDoOnce;
	
	if (InWater)
	{
		if(DifferenceZ > HalfHeight)
		{
			// Do Once Macro
			if(SwimDoOnce)
			{
				WaterVolume = true;
				GetCharacterMovement()->SetMovementMode(MOVE_Swimming);
				SwimDoOnce = !SwimDoOnce;
			}
		}
		
		else
		{
			if(DifferenceZ < HalfHeight)
			{
				if(WalkDoOnce)
				{
					WaterVolume = false;
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
					WalkDoOnce = !WalkDoOnce;
				}
			}
		}
	}
	
}

// Timer function
void ASwimCharacter::CountDown()
{
	if(Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if(Minutes == 0)
		{
			// Game Over depending on rings collected
			IsGameOver = true;
			UGameplayStatics::OpenLevel(this, "GameOver");
		}

		else if(Minutes == 0 && Seconds <= 5)
		{
			SetInstanceVariables();
		}
		
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

void ASwimCharacter::SetInstanceVariables()
{
	CollectedRings = GameInstance->CurrentRings;
	//CurrentRings = CollectedRings;
}


/*void ASwimCharacter::RecuperateStamina()
{
	Stamina++;

	Stamina = FMath::Clamp(Stamina, MinStamina, MaxStamina);

	if(Stamina != MaxStamina)
	{
		if(!IsFastSwimming)
		{
			RecuperateStamina();
		}
	}
}*/

/*void ASwimCharacter::DecreaseStamina()
{
	Stamina--;

	Stamina = FMath::Clamp(Stamina, MinStamina, MaxStamina);
	
	if(Stamina == 0)
	{
		EndFastSwimming();
	}

	else
	{
		if(IsFastSwimming)
		{
			DecreaseStamina();
		}
	}
}*/

/*void ASwimCharacter::StaminaBar()
{
	if (Stamina >= 1)
		Stamina = 1;
	else
	{
		++Stamina;
	}
}*/

void ASwimCharacter::StartFastSwim()
{
	if(Stamina > 10.0f)
	{
		IsFastSwimming = true;
		ControlFastSwimTimer(true);
	}
	
	else if(Stamina <= 0.0f)
	{
		ControlFastSwimTimer(false);
	}

	SwimmingSpeed = 300;
	GetCharacterMovement()->MaxSwimSpeed = SwimmingSpeed;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(SwimmingSpeed));
}

void ASwimCharacter::EndFastSwim()
{
	IsFastSwimming = false;
	ControlFastSwimTimer(false); 
	SwimmingSpeed = 150;
	GetCharacterMovement()->MaxSwimSpeed = SwimmingSpeed;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(SwimmingSpeed));
}

void ASwimCharacter::HandleFastSwim()
{
	if(IsFastSwimming)
	{
		LoseStamina(2.0f);
		if(Stamina <= 0.0f)
			EndFastSwim();
	}
}


void ASwimCharacter::RegenStamina()
{
	
	if(Stamina >= 100.0f)
		Stamina = 100.0f;
	else
	{
		Stamina++;
		//GetWorldTimerManager().UnPauseTimer(StaminaHandle);
	}
}

void ASwimCharacter::LoseStamina(float Value)
{
	if(Stamina - Value < 0.0f)
	{
		Stamina = 0.0f;
	}
	else 
		Stamina -= Value;
	//GetWorldTimerManager().PauseTimer(StaminaHandle);
}

void ASwimCharacter::ControlFastSwimTimer(bool IsFastSwim)
{
	if(IsFastSwimming)
	{
		GetWorldTimerManager().PauseTimer(StaminaHandle);
	}
	else
	{
		GetWorldTimerManager().UnPauseTimer(StaminaHandle);

	}
}


// Code below was supposed to work but it was not being implemented
/*void ASwimCharacter::EnterWater_Implementation()
{
	InWater = true;
	WaterZ = GetActorLocation().Z;
}

void ASwimCharacter::ExitWater_Implementation()
{
	InWater = false;
}*/



