// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Hera.h"
#include "base_ability.generated.h"

/// TODO: Zach - 4/24/23
// - Decide how to deal with communicating character changes with abilities.
//   -- Set GameplayTags on the ASC?
//   -- Make standard callback Events on the base_ability for things like OnAvatarLanded, etc.

UCLASS()
class HERA_API UAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public: 
	UAbilityBase();

	/// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Hera|Ability")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;

	/// Value to associate an ability with an slot without tying it to an automatically activated input.
	/// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hera|Ability")
	EAbilityInputID AbilityID = EAbilityInputID::None;

	/// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities 
	/// forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hera|Ability")
	bool ActivateAbilityOnGranted = false;

	/// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	/// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilitySpec& Spec
	) override;
};
