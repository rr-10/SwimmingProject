// SwimInstance.h
// Written by Remo Reji Thomas (rr-10)

// Code is for the BP_SwimInstance implementation

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SwimInstance.generated.h"

/**
 * The only thing used in the instance was a reference, which is used to display
 * how many rings the player has collected for the game over screen
 */
UCLASS()
class SWIMMINGPROJECT_API USwimInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category=Ring)
	int InstanceRings = 0;
};
