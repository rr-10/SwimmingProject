// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SwimGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SWIMMINGPROJECT_API USwimGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int CurrentRings;
	
};
