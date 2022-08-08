// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "SwimmingProject/SwimInterface.h"
#include "SwimInterface.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "SwimCharacter.generated.h"


UCLASS(config=Game)
class ASwimCharacter : public ACharacter//, public ISwimInterface
{
	GENERATED_BODY()

	// Component Declaration
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ASwimCharacter();

    // Tick, when needed
    virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
protected:
	// Called at start of play session
	virtual void BeginPlay() override;

	// Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);

	// Player setup
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// OBSOLETE FUNCTIONS
	/*
	//UFUNCTION()
	//void Swimming();

	//UFUNCTION()
	//void AscendSwimmingHeight();

	//UFUNCTION()
	//void DescendSwimmingHeight(); 

	// Interface Implementations
	//virtual void EnterWater_Implementation() override;
	//virtual void ExitWater_Implementation() override;

	// UFUNCTION(BlueprintCallable)
	// void SwimmingHeightSet(AActor* ActorObj);
	
	//UFUNCTION()
	//void SetInstanceVariables();
	*/
	
private:

	void RegenStamina();
	void LoseStamina(float Value);
	void ControlSwimmingFastTimer(bool IsSwimFast) const;
	void CountDownTimerFX() const;
	void StartSwimmingFast();
	void StopSwimmingFast();
	void HandleSwimmingFast();
	void CountDown();
	
protected:

	UPROPERTY(BlueprintReadWrite)
	float Stamina;
	//int MinStamina;
	UPROPERTY(BlueprintReadWrite)
	int MaxStamina = 100;

public:
	
	// Bool in place to detect when player is in water
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	bool InWater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float WaterZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor Find")
	TSubclassOf<AActor> FluidClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	USoundCue* CountDownFX;
	
	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Ring")
	int CollectedRings = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Minutes = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	int Seconds = 0;

	//OBSOLETE VARIABLES
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
	//int MaxRings = 5;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	//float SwimmingZ;

private:

	// Audio file reference for the timer SFX
	UAudioComponent* TimerAudioComp;
	bool IsSwimmingFast = false;
	FTimerHandle StaminaHandle;
	FTimerHandle SwimmingHandle;
	FTimerHandle CountDownHandle;
	float SwimmingSpeed;
	
};

