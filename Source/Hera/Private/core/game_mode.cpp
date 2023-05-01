// Copyright Final Fall Games. All Rights Reserved.

#include "core/game_mode.h"
#include "core/actors/base_character_actor.h"
#include "core/base_player_controller.h"

#include "UObject/ConstructorHelpers.h"

AHeraGameMode::AHeraGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter")
	);
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerControllerClass = APlayerControllerBase::StaticClass();
}
