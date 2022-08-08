// Copyright Epic Games, Inc. All Rights Reserved.

#include "SwimCharacter.h"
#include "SwimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetStringLibrary.h"


ASwimCharacter::ASwimCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
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

	PrimaryActorTick.bCanEverTick = true;

	//SwimInstance = Cast<UGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	/*if(SwimInstance != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Game Instance loaded");
	}*/
	
	static ConstructorHelpers::FObjectFinder<USoundCue> CountFXObj (TEXT("SoundCue'/Game/Sounds/5SecsLeft_Cue.5SecsLeft_Cue'"));
	if(CountFXObj.Succeeded())
	{
		CountDownFX = CountFXObj.Object;
		TimerAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TimerAudioComp"));
		TimerAudioComp->SetupAttachment(RootComponent);
	}
}


// Creation of a function that assigns Actions and Axis Values to specific functions
void ASwimCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FastSwim", IE_Pressed, this, &ASwimCharacter::StartSwimmingFast);
	PlayerInputComponent->BindAction("FastSwim", IE_Released, this, &ASwimCharacter::StopSwimmingFast);
	//PlayerInputComponent->BindAction("Ascend", IE_Pressed, this, &ASwimCharacter::DescendSwimmingHeight);
	//PlayerInputComponent->BindAction("Ascend", IE_Pressed, this, &ASwimCharacter::AscendSwimmingHeight);
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASwimCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASwimCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down", this, &APawn::AddControllerPitchInput);
}

void ASwimCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Setting default values on start of game
	Stamina = 100.0f;
	SwimmingSpeed = 200.0f;
	
	// Setting up timers to deal with regeneration of stamina and timers in place
	GetWorldTimerManager().SetTimer(CountDownHandle, this, &ASwimCharacter::CountDown, 1.0f, true);
	GetWorldTimerManager().SetTimer(StaminaHandle, this, &ASwimCharacter::RegenStamina, 1.0f, true);
	GetWorldTimerManager().SetTimer(SwimmingHandle, this, &ASwimCharacter::HandleSwimmingFast, 1.0f, true);

	// If both are declared in the class, then set the sound for TimerAudioComp
	if(TimerAudioComp && CountDownFX)
		TimerAudioComp->SetSound(CountDownFX);
}

// Function that deals with Player moving Forwards/Backwards
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
	}
}

// Function that deals with Player moving Left/Right
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
}

// Function that plays the countdown SFX
void ASwimCharacter::CountDownTimerFX() const
{
	if(Minutes == 0 && Seconds == 5)
		TimerAudioComp->Play(0.f);
}


// Function for the countdown timer
void ASwimCharacter::CountDown()
{
	if(Seconds != 0)
		Seconds = Seconds - 1;
	
	else
	{
		if(Minutes == 0)
			UGameplayStatics::OpenLevel(this, "GameOver");
		
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

// Function that dictates when the player can start to swim fast
void ASwimCharacter::StartSwimmingFast()
{
	// If the player's stamina is less than 10 or higher, then player can fast swim
	if((Stamina > 10.0f)||(Stamina < 10.0f))
	{
		IsSwimmingFast = true;
		ControlSwimmingFastTimer(true);
	}

	// If Stamina ever gets lower than 0, stop swimming fast
	else if(Stamina <= 0.0f)
		ControlSwimmingFastTimer(false);

	// Default value when fast swimming and setting it as the swimming speed from GetCharacterMovement's MaxSwimSpeed
	SwimmingSpeed = 400;
	GetCharacterMovement()->MaxSwimSpeed = SwimmingSpeed;
}

// Function when player is not fast swimming, sets the swimming speed back to 200 and sets MaxSwimSpeed
void ASwimCharacter::StopSwimmingFast()
{
	IsSwimmingFast = false;
	ControlSwimmingFastTimer(false); 
	SwimmingSpeed = 200;
	GetCharacterMovement()->MaxSwimSpeed = SwimmingSpeed;
}

// If the player is swimming fast, lose stamina by 2 but if the stamina gets lower than 0, stop swimming fast
void ASwimCharacter::HandleSwimmingFast()
{
	if(IsSwimmingFast)
	{
		LoseStamina(2.0f);
		if(Stamina <= 0.0f)
			StopSwimmingFast();
	}
}

// If the player is not swimming fast, stamina regains by one otherwise if higher than 100, cap it at 100
void ASwimCharacter::RegenStamina()
{
	if(Stamina >= 100.0f)
		Stamina = 100.0f;
	
	else
		Stamina++;
}

// If player is fast swimming, the stamina will be deducted
// otherwise if stamina is at 0, cap it at 0
void ASwimCharacter::LoseStamina(float Value)
{
	if(Stamina - Value < 0.0f)
		Stamina = 0.0f;
	
	else 
		Stamina -= Value;
}

// Toggle function that deals with pausing stamina timers when swimming fast
void ASwimCharacter::ControlSwimmingFastTimer(bool IsSwimFast) const
{
	IsSwimFast = IsSwimmingFast;
	
	if(IsSwimFast)
		GetWorldTimerManager().PauseTimer(StaminaHandle);

	else
		GetWorldTimerManager().UnPauseTimer(StaminaHandle);
}

// DEPRECATED FUNCTIONS - Some functions were created as a means to not use
// blueprint nodes or functions but it did not translate well due to various errors
// As a result, some were swapped for blueprint implementations or removed

/*
	// Implementation of EnterWater that would execute when in contact with water
	void ASwimCharacter::EnterWater_Implementation()
	{
		InWater = true;
		WaterZ = GetActorLocation().Z;
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::SanitizeFloat(WaterZ));
	}

	// Implementation of ExitWater that would execute when exiting water
	void ASwimCharacter::ExitWater_Implementation()
	{
		InWater = false;
	}
	
	// An attempt at stabilising the swimming height which did not do its intended function
	// This worked better in Blueprint than C++
	void ASwimCharacter::SwimmingHeightSet(AActor* ActorObj)
	{
		//TArray<AActor*> FoundActors;
		//UGameplayStatics::GetAllActorsOfClass(GetWorld(), FluidClass, FoundActors);

		float LocationZ = ActorObj->GetActorLocation().Z;
		
		if(LocationZ > 65)
		{
			SetActorLocation(FVector(ActorObj->GetActorLocation().X, ActorObj->GetActorLocation().Y, GetActorLocation().Z));
		}
	}

	// A C++ function that would do the swimming behaviour once in contact but
	// After three weeks, this approach was not working so it was swapped out to
	// blueprint as it worked better
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
	
	// These were functions that were created that would deal with underwater diving but while
	// they were swapped to Blueprint, underwater diving was unfortunately scrapped due to the
	// buoyancy physics messing it up. Diving underwater was fine but once you got back to the surface,
	// the player flops out and switches movement mode as soon as it goes out of water and as a result,
	// the Z value (even being set to 65 when swimming), was not constant as it went beyond and ended up
	// swimming up to the surface for a long duration of time.
	// Apart from time constraints, the diving had to be scrapped.
	void ASwimCharacter::AscendSwimmingHeight()
	{
		if(GetActorLocation().Z > 65.0f)
		{
			AddMovementInput(FVector(GetActorLocation().X, GetActorLocation().Y, SwimmingZ), 1);
		}

		else
		{
			AddMovementInput(FVector(GetActorLocation().X, GetActorLocation().Y, SwimmingZ), 1);
		}
	}

	void ASwimCharacter::DescendSwimmingHeight()
	{
		AddMovementInput(FVector(0,0,25), -1);
	}

	// Inactive function which would crash the project due to memory allocations
	void ASwimCharacter::SetInstanceVariables()
	{
		CollectedRings = SwimInstance->InstanceRings;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::FromInt(SwimInstance->InstanceRings));
	}

 */

	
	





