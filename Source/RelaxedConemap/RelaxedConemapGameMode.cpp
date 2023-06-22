// Copyright Epic Games, Inc. All Rights Reserved.

#include "RelaxedConemapGameMode.h"
#include "RelaxedConemapCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARelaxedConemapGameMode::ARelaxedConemapGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
