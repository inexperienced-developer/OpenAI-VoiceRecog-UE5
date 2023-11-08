// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenAISphinxGameMode.h"
#include "OpenAISphinxCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOpenAISphinxGameMode::AOpenAISphinxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
