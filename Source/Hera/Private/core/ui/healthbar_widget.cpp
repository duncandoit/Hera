// Copyright Final Fall Games. All Rights Reserved.

#include "core/ui/healthbar_widget.h"
#include "core/actors/base_character_actor.h"
#include "core/debug_utils.h"


void UHealthbarWidget::SetOwningCharacter(ACharacterBase* NewOwningCharacter)
{
   UHeraUtil::DebugPrint("UHealthbarWidget::SetOwningCharacter()", FColor::Purple);
   OwningCharacter =  NewOwningCharacter; 
   SetCurrentHealth(  OwningCharacter->GetHealth());
   SetCurrentShields( OwningCharacter->GetShields());
   SetCurrentArmor(   OwningCharacter->GetArmor());
   SetOverHealth(     OwningCharacter->GetOverHealth());
   SetOverArmor(      OwningCharacter->GetOverArmor());
}