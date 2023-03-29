// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Hera.h"
#include "base_ability.generated.h"


UCLASS()
class HERA_API UAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public: 
	UAbilityBase();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Hera|Ability|Base")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags, 
		const FGameplayTagContainer* TargetTags, 
		FGameplayTagContainer* OptionalRelevantTags
	) const override;
};
