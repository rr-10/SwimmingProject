// Copyright Epic Games, Inc. All Rights Reserved.

/*
 *
 *
 *
 *
 */

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

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ASwimCharacter();

	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

    // Tick, when needed
    virtual void Tick(float DeltaTime) override;

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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ring")
	//int MaxRings = 5;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	//float SwimmingZ;
	
protected:
	// Called at start of play session
	virtual void BeginPlay() override;

	// Functions created to deal with swimming and stamina
	void StartFastSwim();
	void EndFastSwim();
	void HandleFastSwim();
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void CountDown();

	// OBSOLETE FUNCTIONS
	//UFUNCTION()
	//void Swimming();

	//UFUNCTION()
	//void AscendSwimmingHeight();

	//UFUNCTION()
	//void DescendSwimmingHeight(); 

	//virtual void EnterWater_Implementation() override;
	//virtual void ExitWater_Implementation() override;

	//UFUNCTION(BlueprintImplementableEvent)
	//void EnterWat();

	// UFUNCTION(BlueprintCallable)
	// void SwimmingHeightSet(AActor* ActorObj);
	
	//UFUNCTION()
	//void SetInstanceVariables();

	void RegenStamina();
	void LoseStamina(float Value);
	void ControlFastSwimTimer(bool IsFastSwimming);

	UPROPERTY(BlueprintReadWrite)
	float Stamina;
	float SwimmingSpeed;
	//int MinStamina;

	UPROPERTY(BlueprintReadWrite)
	int MaxStamina = 100;
	
	FTimerHandle StaminaHandle;
	FTimerHandle SwimmingHandle;



	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	// Audio file reference for the timer SFX
	UAudioComponent* TimerAudioComp;
	bool IsFastSwimming = false;
	
};

