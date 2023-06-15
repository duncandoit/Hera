// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/base_asc.h"

void UAbilitySystemComponentBase::OnReceivedDamage(
	UAbilitySystemComponentBase* SourceASC, 
	float FinalDamage,
	bool bIsDead
)
{
	DamageReceivedDelegate.Broadcast(SourceASC, FinalDamage, bIsDead);
}

void UAbilitySystemComponentBase::OnReceivedHealing(
	UAbilitySystemComponentBase* SourceASC, 
	float FinalHealing
)
{
	HealingReceivedDelegate.Broadcast(SourceASC, FinalHealing);
}