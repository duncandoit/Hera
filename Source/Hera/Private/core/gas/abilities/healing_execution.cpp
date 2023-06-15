// Copyright Final Fall Games. All Rights Reserved.

#include "core/gas/abilities/healing_execution.h"
#include "core/gas/life_attribute_set.h"
#include "core/gas/base_asc.h"
#include "core/gas/tags.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct FHealingCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Healing);

	FHealingCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(ULifeAttributeSet, Healing, Source, true);
	}
};

static const FHealingCapture& GetHealingCapture()
{
	static FHealingCapture HealingCapture;
	return HealingCapture;
}

UHealingExecution::UHealingExecution()
{
	CritMultiplier = 2.0f;

	RelevantAttributesToCapture.Add(GetHealingCapture().HealingDef);
}

void UHealingExecution::Execute_Implementation(
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

	auto Healing = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetHealingCapture().HealingDef, 
		EvaluationParameters, 
		Healing
	);
	// Add SetByCaller Healing if it exists
	Healing += FMath::Max<float>(
		EffectSpec.GetSetByCallerMagnitude(HeraTags::Tag_Healing, false, -1.0f), // tag, warn, default
		0.0f
	);

	// Can multiply any Healing boosters here
	float UnmitigatedHealing = Healing; 
	
	const float HealingMitigated = 0;
	const float FinalHealing = UnmitigatedHealing - HealingMitigated;

	if (FinalHealing > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			GetHealingCapture().HealingProperty, 
			EGameplayModOp::Additive, 
			FinalHealing
		));

		// Broadcast to the Target's ASC
		if (const auto TargetHeroASC = Cast<UAbilitySystemComponentBase>(TargetASC))
		{
			const auto SourceHeroASC = Cast<UAbilitySystemComponentBase>(SourceASC);
			TargetHeroASC->OnReceivedHealing(SourceHeroASC, FinalHealing);
		}
	}
}
