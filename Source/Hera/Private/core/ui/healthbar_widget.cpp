// Copyright Final Fall Games. All Rights Reserved.

#include "core/ui/healthbar_widget.h"
#include "core/actors/base_character_actor.h"


void UHealthbarWidget::SetOwningCharacter(ACharacterBase* NewOwningCharacter)
{
   OwningCharacter =  NewOwningCharacter; 
   SetCurrentHealth(  OwningCharacter->GetHealth());
   SetCurrentShields( OwningCharacter->GetShields());
   SetCurrentArmor(   OwningCharacter->GetArmor());
   SetOverHealth(     OwningCharacter->GetOverHealth());
   SetOverArmor(      OwningCharacter->GetOverArmor());
}