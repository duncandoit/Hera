// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/hero_asc.h"

void UHeroAbilitySystemComponent::ReceiveDamage(
   UHeroAbilitySystemComponent* SourceASC, 
   float UnmitigatedDamage, 
   float MitigatedDamage
)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}