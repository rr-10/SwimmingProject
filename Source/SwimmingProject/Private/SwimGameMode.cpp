// Fill out your copyright notice in the Description page of Project Settings.


#include "SwimGameMode.h"
#include "SwimCharacter.h"
#include "UObject/ConstructorHelpers.h"

// The only thing this code does it sets the character as the default pawn class
// The blueprint class is mainly used, which derives from this class
ASwimGameMode::ASwimGameMode()
{
	DefaultPawnClass = ASwimCharacter::StaticClass();
}

