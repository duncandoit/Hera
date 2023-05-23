// Copyright Final Fall Games. All Rights Reserved.

#include "core/game_mode.h"
#include "core/actors/base_hero.h"
// #include "core/actors/base_character_actor.h"
#include "core/base_player_controller.h"

#include "UObject/ConstructorHelpers.h"

AHeraGameMode::AHeraGameMode()
	: Super()
{
	// set default pawn class
	// static ConstructorHelpers::FClassFinder<APawn> PawnClassFinder(
		// TEXT("/Game/Hera/Characters/HERO_Test.HERO_Test_C")
		// TEXT("/Game/Hera/Characters/CHAR_Test.CHAR_Test_C")
		// TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter.BP_FirstPersonCharacter_C")
	// );
	// DefaultPawnClass = PawnClassFinder.Class;
	// DefaultPawnClass = AHeroBase::StaticClass();

	PlayerControllerClass = APlayerControllerBase::StaticClass();
}
