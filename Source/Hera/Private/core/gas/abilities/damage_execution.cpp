// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/damage_execution.h"
#include "core/gas/life_attribute_set.h"
#include "core/gas/hero_asc.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct DamageStatics
{
	// Capturing Attributes:
	// The attributes captured should be things that will affect the outcome of the calculation but
	// may change from moment to moment (like AttackPower, if it can be buffed by another ability, say)
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(OverArmor);

	DamageStatics()
	{
		// Note: On Snapshotting
		//Snapshotting captures the Attribute when the GameplayEffectSpec is created whereas not snapshotting 
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

static const DamageStatics& gkDamageStatics()
{
	static DamageStatics DAMAGE_STATICS;
	return DAMAGE_STATICS;
}

UDamageExecution::UDamageExecution()
{
	CritMultiplier = 2.0f;

	RelevantAttributesToCapture.Add(gkDamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(gkDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(gkDamageStatics().OverArmorDef);
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

	auto Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		gkDamageStatics().ArmorDef, 
		EvaluationParameters, 
		Armor
	);
	Armor = FMath::Max<float>(Armor, 0.0f);

	auto OverArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		gkDamageStatics().OverArmorDef, 
		EvaluationParameters, 
		OverArmor
	);
	OverArmor = FMath::Max<float>(OverArmor, 0.0f);

	auto Damage = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		gkDamageStatics().DamageDef, 
		EvaluationParameters, 
		Damage
	);
	// Add SetByCaller damage if it exists
	// Damage += FMath::Max<float>(
	// 	// DataName, WarnIfNotFound, DefaultIfNotFound
	// 	EffectSpec.GetSetByCallerMagnitude(
	// 		FGameplayTag::RequestGameplayTag(FName("Data.Damage")), 
	// 		false, 
	// 		-1.0f
	// 	), 
	// 	0.0f
	// );

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
			gkDamageStatics().DamageProperty, 
			EGameplayModOp::Additive, 
			FinalDamage
		));

		// Broadcast to the Target's ASC
		if (const auto TargetHeroASC = Cast<UHeroAbilitySystemComponent>(TargetASC))
		{
			const auto SourceHeroASC = Cast<UHeroAbilitySystemComponent>(SourceASC);
			TargetHeroASC->OnReceivedDamage(SourceHeroASC, UnmitigatedDamage, FinalDamage);
		}
	}
}
