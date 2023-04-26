// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/damage_execution.h"
#include "core/gas/life_attribute_set.h"
#include "core/gas/base_asc.h"
#include "core/gas/tags.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FDamageCapture
{
	// Capturing Attributes:
	// The attributes are captured with their BaseValue.
	// They hould be things that will affect the outcome of the calculation but may change from moment to moment 
	// (like AttackPower, if it can be buffed by another ability, say)
	
	// Source Attributes
	// DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityDamage);

	// Target Attributes
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(OverArmor);

	FDamageCapture()
	{
		/// NOTE: On Snapshotting
		// Snapshotting captures the Attribute when the GameplayEffectSpec is created whereas not snapshotting 
		// captures the Attribute when the GameplayEffectSpec is applied. Capturing Attributes recalculates their 
		// CurrentValue from existing mods on the ASC. This recalculation will not run PreAttributeChange() in the 
		// AbilitySet so any clamping must be done here again.

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(ULifeAttributeSet, Damage, Source, true);

		// Capture the Target's Armor. Don't snapshot.
		DEFINE_ATTRIBUTE_CAPTUREDEF(ULifeAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(ULifeAttributeSet, OverArmor, Target, false);

		/// TODO: When the other stats are implemented we may want to capture things lke AttackStat from the Source
		// or DefenseStat from the Target, etc.
	}
};

static const FDamageCapture& GetDamageCapture()
{
	static FDamageCapture DamageCapture;
	return DamageCapture;
}

UDamageExecution::UDamageExecution()
{
	CritMultiplier = 2.0f;

	RelevantAttributesToCapture.Add(GetDamageCapture().DamageDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().OverArmorDef);
}

void UDamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	auto TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	auto SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	FGameplayTagContainer AssetTags;
	EffectSpec.GetAllAssetTags(AssetTags);

	// Gather the tags from the source and target as that can affect which buffs should be used
	const auto SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const auto TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetDamageCapture().ArmorDef, 
		EvaluationParameters, 
		Armor
	);
	Armor = FMath::Max<float>(Armor, 0.0f);

	float OverArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetDamageCapture().OverArmorDef, 
		EvaluationParameters, 
		OverArmor
	);
	OverArmor = FMath::Max<float>(OverArmor, 0.0f);

	// Grabs the value from the attribute set, goes through Effects and see if anything will change 
	// the Damage attribute, and adds it to the Damage float.
	// Value set on the damage GE as a CalculationModifier under the ExecutionCalculation.
	float Damage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetDamageCapture().DamageDef, 
		EvaluationParameters, 
		Damage
	);
	
	//Add SetByCaller damage if it exists
	const auto SetByTag = HeraTags::Tag_Damage;
	const float SetByDamage = EffectSpec.GetSetByCallerMagnitude(SetByTag, false, -1.0f); // tag, warn, default
	Damage += FMath::Max<float>(SetByDamage, 0.0f);

	// Can multiply any damage boosters here
	float UnmitigatedDamage = Damage; 

	// Check for headshot. There's only one character mesh here, but you could have a function on your Character 
	// class to return the head bone name
	// const auto Hit = EffectSpec.GetContext().GetHitResult();
	// const auto CanHeadShot = AssetTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Effect.Damage.CanHeadShot")));
	// if (CanHeadShot && Hit && Hit->BoneName == "b_head")
	// {
	// 	UnmitigatedDamage *= CritMultiplier;
	// 	auto MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	// 	MutableSpec->DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Effect.Damage.HeadShot")));
	// }
	
	// All Armor in the Health Pool will mitigate 50% of the incoming damage
	const float DamageMitigated = FMath::Min<float>(UnmitigatedDamage, (Armor + OverArmor));
	const float FinalDamage = UnmitigatedDamage - DamageMitigated + (DamageMitigated * 0.5f);

	if (FinalDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			GetDamageCapture().DamageProperty, 
			EGameplayModOp::Additive, 
			FinalDamage
		));

		// If we want to trigger conditional Effects when the execution is successful.
		// OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();

		// If we handle GameplayCues manually.
		// disable the GE from firing its cues after the calculation
		// OutExecutionOutput.MarkGameplayCuesHandledManually();

		// Broadcast to the Target's ASC
		if (const auto TargetHeroASC = Cast<UAbilitySystemComponentBase>(TargetASC))
		{
			const auto SourceHeroASC = Cast<UAbilitySystemComponentBase>(SourceASC);
			TargetHeroASC->OnReceivedDamage(SourceHeroASC, UnmitigatedDamage, FinalDamage);
		}
	}

	// If we handle the stack count manually.
	// The GE system should not attempt to automatically act upon it for emitted modifiers.
	// OutExecutionOutput.MarkStackCountHandledManually();
}
