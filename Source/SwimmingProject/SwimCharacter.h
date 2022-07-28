// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "SwimmingProject/SwimInterface.h"
#include "SwimCharacter.generated.h"

UCLASS(config=Game)
class ASwimCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Create trigger capsule
	//UPROPERTY(VisibleAnywhere, Category = "Trigger Capsule")
	//class UCapsuleComponent* TriggerCapsule;

public:
	ASwimCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

    // Tick, when needed
    virtual void Tick(float DeltaTime) override;
    
    // float GetAirValue() const { return AirValue; }
	// void SetAirValue(float val) { AirValue = val; }

	// float GetMaxAir() const { return MaxAirValue; }
	// void SetMaxAir(float val) { MaxAirValue = val; }
	
	// Interface override functions
	// void EnterWater_Implementation() override;
	// void ExitWater_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	bool InWater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float WaterZ;

	bool SwimDoOnce, WalkDoOnce;

	FTimerHandle SwimmingTimerHandle;

protected:

	virtual void BeginPlay() override;
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
	void Swimming();

	// UPROPERTY(VisibleAnywhere)
	// class UWidgetComponent* AirWidget;
	//
	// float AirValue, MaxAirValue = 30;

	void CountDown();

	UPROPERTY(BlueprintReadOnly)
	int Minutes = 2;

	UPROPERTY(BlueprintReadOnly)
	int Seconds = 0;

	UPROPERTY(BlueprintReadOnly)
	int MaxRings = 5;

	UPROPERTY(BlueprintReadWrite)
	int CollectedRings = 0;

	void StaminaBar();
	
	UPROPERTY(BlueprintReadWrite)
	float Stamina = 0;

	FTimerHandle StaminaHandle;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

