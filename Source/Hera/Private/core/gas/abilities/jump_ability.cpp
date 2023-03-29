// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/jump_ability.h"
#include "core/actors/character_actor.h"
#include "Hera.h"

UJumpAbility::UJumpAbility()
{
	AbilityInputID = EAbilityInputID::Jump;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UJumpAbility::ActivateAbility(
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

		ACharacter* Avatar = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
		Avatar->Jump();
	}
}

bool UJumpAbility::CanActivateAbility(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayTagContainer* SourceTags, 
   const FGameplayTagContainer* TargetTags, 
   OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const AHeraCharacter* Avatar = CastChecked<AHeraCharacter>(
      ActorInfo->AvatarActor.Get(), 
      ECastCheckedType::NullAllowed
   );

	return Avatar && Avatar->CanJump();
}

void UJumpAbility::InputReleased(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayAbilityActivationInfo ActivationInfo
)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

// Epic's comment:
//
//	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go 
// wrong by calling StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability,
// it would need to make sure the Montage that *it* played was still playing, and if so, to cancel it. If this is 
// something we need to support, we may need some light weight data structure to represent 'non intanced abilities 
// in action' with a way to cancel/end them.
//
void UJumpAbility::CancelAbility(
   const FGameplayAbilitySpecHandle Handle, 
   const FGameplayAbilityActorInfo* ActorInfo, 
   const FGameplayAbilityActivationInfo ActivationInfo, 
   bool bReplicateCancelAbility
)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(
         this, 
         &UJumpAbility::CancelAbility, 
         Handle, 
         ActorInfo, 
         ActivationInfo, 
         bReplicateCancelAbility
      ));

		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}
