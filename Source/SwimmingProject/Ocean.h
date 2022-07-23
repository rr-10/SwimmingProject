// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SwimInterface.h"
#include "WaterBodyOceanActor.h"
#include "Ocean.generated.h"

/**
 * 
 */
UCLASS()
class SWIMMINGPROJECT_API AOcean : public AWaterBodyOcean, public ISwimInterface
{
	GENERATED_BODY()

	

public:
	virtual void EnterWater_Implementation() override;
	//virtual void ExitWater_Implementation() override;
	
	UPROPERTY(BlueprintReadWrite)
	UObject* SwimInterfaceInstance;
	
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void EnterWaterMessage();
	
};
