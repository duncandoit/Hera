// Copyright Final Fall Games. All Rights Reserved.

#include "game_mode.h"
#include "actors/character_actor.h"

#include "UObject/ConstructorHelpers.h"

AHeraGameMode::AHeraGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
