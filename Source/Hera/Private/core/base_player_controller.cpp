// Copyright Final Fall Games. All Rights Reserved.

#include "core/actors/base_character_actor.h"
#include "core/base_player_controller.h"

#include "GameFramework/PlayerState.h"

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

   // auto PossessedCharacter = Cast<ACharacterBase>(InPawn);
   // if (PossessedCharacter)
   // {
   //    PossessedCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(this, InPawn);
   // }

	// auto State = GetPlayerState<APlayerStateBase>();
	// if (State)
	// {
	// 	// Init ASC with PlayerState (Owner) and our new Pawn (AvatarActor)
	// 	State->GetAbilitySystemComponent()->InitAbilityActorInfo(State, InPawn);
	// }
}