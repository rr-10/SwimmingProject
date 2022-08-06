// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SwimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SWIMMINGPROJECT_API USwimInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category=Ring)
	int InstanceRings = 0;
};
