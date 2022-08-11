// SwimGameMode.cpp
// Written by Remo Reji Thomas (rr-10)

// Code is for the BP_SwimGameMode implementation

#include "SwimGameMode.h"
#include "SwimCharacter.h"
#include "UObject/ConstructorHelpers.h"

// The only thing this code does it sets the character as the default pawn class
// The blueprint class is mainly used, which derives from this class
ASwimGameMode::ASwimGameMode()
{
	DefaultPawnClass = ASwimCharacter::StaticClass();
}

