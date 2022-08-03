// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "SwimmingProject/SwimInterface.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
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

public:
	ASwimCharacter();
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

    // Tick, when needed
    virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	bool InWater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float WaterZ;

	bool SwimDoOnce, WalkDoOnce;

	class USwimInstance* SwimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	USoundCue* CountDownFX;

protected:
	
	virtual void BeginPlay() override;

	void StartFastSwim();
	void EndFastSwim();
	void HandleFastSwim();
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	UFUNCTION()
	void Swimming();

	void CountDown();

	UPROPERTY(BlueprintReadOnly)
	int Minutes = 2;

	UPROPERTY(BlueprintReadOnly)
	int Seconds = 0;

	UPROPERTY(BlueprintReadOnly)
	int MaxRings = 5;

	UPROPERTY(BlueprintReadWrite)
	int CollectedRings = 0;

	bool IsGameOver;
	
	//UFUNCTION()
	//void SetInstanceVariables();

	void RegenStamina();
	void LoseStamina(float Value);
	void ControlFastSwimTimer(bool IsFastSwimming);

	UPROPERTY(BlueprintReadWrite)
	//int Stamina = 100;
	float Stamina;
	float SwimmingSpeed;
	int MinStamina;

	UPROPERTY(BlueprintReadWrite)
	int MaxStamina = 100;

	FTimerHandle StaminaHandle;
	FTimerHandle SwimmingHandle;

	UPROPERTY(BlueprintReadWrite)
	bool IsFastSwimming = false;
	
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

	UAudioComponent* TimerAudioComp;
};

