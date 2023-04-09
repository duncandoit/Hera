// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/hero_asc.h"

void UHeroAbilitySystemComponent::OnReceivedDamage(
   UHeroAbilitySystemComponent* SourceASC, 
   float UnmitigatedDamage, 
   float FinalDamage
)
{
	DamageReceivedDelegate.Broadcast(SourceASC, UnmitigatedDamage, FinalDamage);
}