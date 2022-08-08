// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SwimInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USwimInterface : public UInterface
{
	GENERATED_BODY()
};

class SWIMMINGPROJECT_API ISwimInterface
{
	GENERATED_BODY()

public:

	// These functions are implemented in the BP_Ocean and BP_SwimCharacter blueprints
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Swim)
	void EnterWater();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Swim)
	void ExitWater();
};
