// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "hero_asc.generated.h"

// These are formatted with a new delegate name first, then the specified number 
// of params listed: param type, param name
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FReceivedDamageDelegate, 
	/*param 1*/UHeroAbilitySystemComponent*, SourceASC, 
	/*param 2*/float, UnmitigatedDamage, 
	/*param 3*/float, MitigatedDamage
);

UCLASS()
class HERA_API UHeroAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;

	// Called from GDDamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(
		UHeroAbilitySystemComponent* SourceASC, 
		float UnmitigatedDamage, 
		float MitigatedDamage
	);
};
