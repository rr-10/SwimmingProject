// Fill out your copyright notice in the Description page of Project Settings.


#include "SwimGameMode.h"
#include "SwimCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASwimGameMode::ASwimGameMode()
{
	DefaultPawnClass = ASwimCharacter::StaticClass();
}

