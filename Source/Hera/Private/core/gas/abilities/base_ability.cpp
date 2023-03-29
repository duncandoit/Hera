// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/base_ability.h"
#include "core/actors/character_actor.h"

UBaseAbility::UBaseAbility()
{
   AbilityInputID = EAbilityInputID::Jump;
}

bool UBaseAbility::CanActivateAbility(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayTagContainer* SourceTags, 
   const FGameplayTagContainer* TargetTags, 
   FGameplayTagContainer* OptionalRelevantTags
) const
{
   return true;
}

void UBaseAbility::ActivateAbility(
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