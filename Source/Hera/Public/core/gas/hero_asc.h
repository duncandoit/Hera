// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "hero_asc.generated.h"

/// These are formatted with a new delegate name first, then the
/// specified number of params listed: param type, param name
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FDamageReceivedDelegate, 
	/*param 1*/UHeroAbilitySystemComponent*, SourceASC, 
	/*param 2*/float, UnmitigatedDamage, 
	/*param 3*/float, FinalDamage
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FHealingReceivedDelegate, 
	/*param 1*/UHeroAbilitySystemComponent*, SourceASC, 
	/*param 2*/float, UnmitigatedHealing, 
	/*param 3*/float, FinalHealing
);

UCLASS()
class HERA_API UHeroAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	/// Broadcasts whenever the ASC receives damage.
	FDamageReceivedDelegate DamageReceivedDelegate;

	/// Broadcasts whenever the ASC receives healing.
	FHealingReceivedDelegate HealingReceivedDelegate;

	/// Called from UDamageExecute. Broadcasts to DamageReceivedDelegate whenever this ASC receives damage.
	virtual void OnReceivedDamage(
		UHeroAbilitySystemComponent* SourceASC, 
		float UnmitigatedDamage, 
		float FinalDamage
	);

	/// Called from UHealingExecution. Broadcasts to DamageReceivedDelegate whenever this ASC receives damage.
	virtual void OnReceivedHealing(
		UHeroAbilitySystemComponent* SourceASC, 
		float UnmitigatedHealing, 
		float FinalHealing
	);
};
