// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/base_ability.h"
#include "core/actors/base_character_actor.h"

UAbilityBase::UAbilityBase()
{
   AbilityInputID = EAbilityInputID::Jump;
}

bool UAbilityBase::CanActivateAbility(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayTagContainer* SourceTags, 
   const FGameplayTagContainer* TargetTags, 
   FGameplayTagContainer* OptionalRelevantTags
) const
{
   return true;
}

void UAbilityBase::ActivateAbility(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayAbilityActivationInfo ActivationInfo, 
   const FGameplayEventData* TriggerEventData
)
{
   if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		Character->Jump();
	}
}