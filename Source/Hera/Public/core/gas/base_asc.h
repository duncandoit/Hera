// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "base_asc.generated.h"

/// These are formatted with a new delegate name first, then the
/// specified number of params listed: param type, param name
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FDamageReceivedDelegate, 
	/*param 1*/UAbilitySystemComponentBase*, SourceASC,
	/*param 2*/float, FinalDamage,
	/*param 3*/bool, bIsDead
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FHealingReceivedDelegate, 
	/*param 1*/UAbilitySystemComponentBase*, SourceASC, 
	/*param 2*/float, FinalHealing
);

UCLASS()
class HERA_API UAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool bDefaultAbilitiesInitialized = false;
	bool bDefaultEffectsInitialized = false;

	/// Broadcasts whenever the ASC receives damage.
	FDamageReceivedDelegate DamageReceivedDelegate;

	/// Broadcasts whenever the ASC receives healing.
	FHealingReceivedDelegate HealingReceivedDelegate;

	/// Called from UDamageExecute. Broadcasts to DamageReceivedDelegate whenever this ASC receives damage.
	virtual void OnReceivedDamage(
		UAbilitySystemComponentBase* SourceASC, 
		float FinalDamage,
		bool bIsDead
	);

	/// Called from UHealingExecution. Broadcasts to HealingReceivedDelegate whenever this ASC receives damage.
	virtual void OnReceivedHealing(
		UAbilitySystemComponentBase* SourceASC, 
		float FinalHealing
	);
};
