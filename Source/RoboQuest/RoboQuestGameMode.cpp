// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoboQuestGameMode.h"
#include "RoboQuestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARoboQuestGameMode::ARoboQuestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
