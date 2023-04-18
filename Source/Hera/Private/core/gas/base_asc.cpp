// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/base_asc.h"

void UAbilitySystemComponentBase::OnReceivedDamage(
   UAbilitySystemComponentBase* SourceASC, 
   float UnmitigatedDamage, 
   float FinalDamage
)
{
	DamageReceivedDelegate.Broadcast(SourceASC, UnmitigatedDamage, FinalDamage);
}

void UAbilitySystemComponentBase::OnReceivedHealing(
   UAbilitySystemComponentBase* SourceASC, 
   float UnmitigatedHealing, 
   float FinalHealing
)
{
	HealingReceivedDelegate.Broadcast(SourceASC, UnmitigatedHealing, FinalHealing);
}