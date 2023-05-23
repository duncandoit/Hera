// Copyright Final Fall Games. All Rights Reserved.

#include "core/ui/healthbar_widget.h"
#include "core/actors/base_character.h"


void UHealthbarWidget::SetOwningCharacter(ACharacterBaseValid* NewOwningCharacter)
{
   OwningCharacter =  NewOwningCharacter; 
   SetCurrentHealth(  OwningCharacter->GetHealth());
   SetCurrentShields( OwningCharacter->GetShields());
   SetCurrentArmor(   OwningCharacter->GetArmor());
   SetOverHealth(     OwningCharacter->GetOverHealth());
   SetOverArmor(      OwningCharacter->GetOverArmor());
}
