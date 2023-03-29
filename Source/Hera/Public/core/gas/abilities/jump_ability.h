// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "core/gas/abilities/base_ability.h"
#include "jump_ability.generated.h"

UCLASS()
class HERA_API UJumpAbility : public UAbilityBase
{
	GENERATED_BODY()
	
public:
	UJumpAbility();

	virtual void ActivateAbility(
      const FGameplayAbilitySpecHandle Handle, 
      const FGameplayAbilityActorInfo* ActorInfo, 
      const FGameplayAbilityActivationInfo ActivationInfo, 
      const FGameplayEventData* TriggerEventData
   ) override;

	virtual bool CanActivateAbility(
      const FGameplayAbilitySpecHandle Handle, 
      const FGameplayAbilityActorInfo* ActorInfo, 
      const FGameplayTagContainer* SourceTags = nullptr, 
      const FGameplayTagContainer* TargetTags = nullptr, 
      OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
   ) const override;

	virtual void InputReleased(
      const FGameplayAbilitySpecHandle Handle, 
      const FGameplayAbilityActorInfo* ActorInfo, 
      const FGameplayAbilityActivationInfo ActivationInfo
   ) override;

	virtual void CancelAbility(
      const FGameplayAbilitySpecHandle Handle, 
      const FGameplayAbilityActorInfo* ActorInfo, 
      const FGameplayAbilityActivationInfo ActivationInfo, 
      bool bReplicateCancelAbility
   ) override;
};