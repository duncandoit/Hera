// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/life_attribute_set.h"
#include "core/gas/base_asc.h"
#include "core/actors/base_character.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

ULifeAttributeSet::ULifeAttributeSet()
{
	// HeadShotTag = FGameplayTag::RequestGameplayTag(FName("Effect.Damage.HeadShot"));
}

void ULifeAttributeSet::HandleDamage(const float DamageReceived /*, SourceCharacterTags*/)
{
	/// TODO: Check for GameplayTags like:
	//       - Immortal       : Can't fall below 1hp
	//       - Invulnerable   : Doesn't take any damage or suffer from debuffs
	//       - Weakened       : Takes more damage
	//       - Fortified      : Takes less damage

	const float OldHealth     = GetHealth();
	if (DamageReceived <= 0.0f || OldHealth <= 0.0f) {
		return;
	}

	// Starting values
	float RemainingDamage     = DamageReceived;
	const float OldOverArmor  = GetOverArmor();
	const float OldOverHealth = GetOverHealth();
	const float OldArmor      = GetArmor();
	const float OldShields    = GetShields();

	// The amount of damage that can be applied to a 
	auto DamagedValue = [&RemainingDamage](const float& OldValue) -> float
	{
		const float AttributeDamage = FMath::Min<float>(OldValue, RemainingDamage);
		const float NewValue = OldValue - AttributeDamage;
		RemainingDamage -= AttributeDamage;
		return FMath::Clamp<float>(NewValue, 0.0f, OldValue);
	};

	// OverArmor
	if (OldOverArmor > 0.0f)
	{
		SetOverArmor(DamagedValue(OldOverArmor));
	}
	
	// OverHealth
	if (OldOverHealth > 0.0f && RemainingDamage > 0.0f)
	{
		SetOverHealth(DamagedValue(OldOverHealth));
	}

	// Armor
	if (OldArmor > 0.0f && RemainingDamage > 0.0f)
	{
		SetArmor(DamagedValue(OldArmor));
	}

	// Shields
	if (OldShields > 0.0f && RemainingDamage > 0.0f)
	{
		SetShields(DamagedValue(OldShields));
	}

	// Health
	if (OldHealth > 0.0f && RemainingDamage > 0.0f)
	{
		SetHealth(DamagedValue(OldHealth));
	}
}

void ULifeAttributeSet::HandleHealing(const float HealingReceived)
{
	/// TODO: Check for GameplayTags like:
	//       - Cursed : Can't receive healing

	const float OldHealth  = GetHealth();
	if (HealingReceived <= 0.0f || OldHealth <= 0.0f) 
	{
		return;
	}

	// Starting values
	float RemainingHealing = HealingReceived;
	const float OldShields = GetShields();
	const float OldArmor   = GetArmor();
	const float HealthMax  = GetMaxHealth();
	const float ShieldsMax = GetMaxShields();
	const float ArmorMax   = GetMaxArmor();
	
	auto HealedValue = [&RemainingHealing](const float& OldValue, const float& MaxValue) -> float
	{
		const float AttributeHealing = FMath::Min<float>(MaxValue - OldValue, RemainingHealing);
		const float NewValue = OldValue + AttributeHealing;
		RemainingHealing -= AttributeHealing;
		return FMath::Clamp<float>(NewValue, OldValue, MaxValue);
	};

	// Health
	if (OldHealth < HealthMax)
	{
		SetHealth(HealedValue(OldHealth, HealthMax));
	}

	// Shields
	if (OldShields < ShieldsMax && RemainingHealing > 0.0f)
	{
		SetShields(HealedValue(OldShields, ShieldsMax));
	}

	// Armor
	if (OldArmor < ArmorMax && RemainingHealing > 0.0f)
	{
		SetArmor(HealedValue(OldArmor, ArmorMax));
	}
}

void ULifeAttributeSet::HandleKillReward(UAbilitySystemComponent* SourceASC)
{
	// Create a dynamic instant Gameplay Effect to give the bounties
	auto BountyEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
	BountyEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	int32 BountyModInfoCount = BountyEffect->Modifiers.Num();

	// This should add the number of Attributes being modified. (XP, gold, etc)
	const int NewModsCount = 1; // XP
	BountyEffect->Modifiers.SetNum(BountyModInfoCount + NewModsCount);

	// XP Reward for the Source
	FGameplayModifierInfo& InfoXP = BountyEffect->Modifiers[BountyModInfoCount];
	InfoXP.ModifierMagnitude = FScalableFloat(GetRewardXP());
	InfoXP.ModifierOp = EGameplayModOp::Additive;
	InfoXP.Attribute = ULifeAttributeSet::GetXPAttribute();

	// Potential gold reward for the Source
	// FGameplayModifierInfo& InfoGold = BountyEffect->Modifiers[BountyModInfoCount + 1];
	// InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
	// InfoGold.ModifierOp = EGameplayModOp::Additive;
	// InfoGold.Attribute = ULifeAttributeSet::GetGoldAttribute();

	// Apply the reward to the killer
	SourceASC->ApplyGameplayEffectToSelf(BountyEffect, 1.0f, SourceASC->MakeEffectContext());
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - UAttributeSet overrides
//---------------------------------------------------------------------------------------------------------------------

void ULifeAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Notes:
   // - Use this mainly for clamping values that are about to change.
   // - The getters were created in the Macros defined at the top of the .h

	Super::PreAttributeChange(Attribute, NewValue);

	// Handle scaling values to new maximum. 
	// If a Max value changes, adjust current to keep Current % of Current to Max.
	if (Attribute == GetMaxHealthAttribute()) 
	{
		AdjustAttributeOnMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
   if (Attribute == GetMaxShieldsAttribute()) 
	{
		AdjustAttributeOnMaxChange(Shields, MaxShields, NewValue, GetShieldsAttribute());
	}
   if (Attribute == GetMaxArmorAttribute()) 
	{
		AdjustAttributeOnMaxChange(Armor, MaxArmor, NewValue, GetArmorAttribute());
	}

	// Handle move speed clamping
	else if (Attribute == GetMoveSpeedAttribute())
	{
      // Move speed in cm/s
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
}

void ULifeAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// The ExecutionCalculation in the ExecutionDefinition is called in Execute,
	// then we come here.

   Super::PostGameplayEffectExecute(Data);

	auto Context = Data.EffectSpec.GetContext();
	auto SourceASC = Context.GetOriginalInstigatorAbilitySystemComponent();
	
	const auto& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ACharacterBaseValid* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacterBaseValid>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	// ACharacterBaseValid* SourceCharacter = nullptr;
	if (SourceASC && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (auto Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// // Get Source Pawn with their controller
		// if (SourceController)
		// {
		// 	SourceCharacter = Cast<ACharacterBaseValid>(SourceController->GetPawn());
		// }
		// else
		// {
		// 	SourceCharacter = Cast<ACharacterBaseNEW>(SourceActor);
		// }

		// Get Source Actor if the context has a causer set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	//----------------------------------------------------------------------------------------
	/// MARK: Handle Damage/Healing
	//----------------------------------------------------------------------------------------

	/// DAMAGE:
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Total damage received
		const float DamageReceived = GetDamage();
		SetDamage(0.f); //clear the damage meta attribute

		if (DamageReceived > 0.0f)
		{
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;
			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			HandleDamage(DamageReceived);

			// Broadcast damage
			// UAbilitySystemComponent* ASC = &Data.Target;
			UAbilitySystemComponentBase* TargetASC = Cast<UAbilitySystemComponentBase>(&Data.Target);
			TargetASC->OnReceivedDamage(Cast<UAbilitySystemComponentBase>(SourceASC), DamageReceived, !TargetCharacter->IsAlive());

			// Post-damage effects
			if (WasAlive)
			{
				/// TODO: Feedback to SourceController
			
				// Check if TargetCharacter was killed
				if (!TargetCharacter->IsAlive())
				{
					// Don't give bounty to self
					if (SourceController != TargetController)
					{
						HandleKillReward(SourceASC);
					}
				}
			}
		}
	}// Damage

	/// HEALING:
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float HealingReceived = GetHealing();
		SetHealing(0.0f);
		HandleHealing(HealingReceived);
	} // Healing

   //----------------------------------------------------------------------------------------
	/// MARK: Handle manual Attribute changes
	//        : These changes come from the ExecutionCalculation.
	//        : Normal Health, Shields, Armor, etc loss/gain should go through Damage/Healing.
	//----------------------------------------------------------------------------------------
	
	// Health
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	
	// Shields
	else if (Data.EvaluatedData.Attribute == GetShieldsAttribute())
	{
		SetShields(FMath::Clamp(GetShields(), 0.0f, GetMaxShields()));
	}

	// Armor
	else if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), 0.0f, GetMaxArmor()));
	}

	// OverHealth
	else if (Data.EvaluatedData.Attribute == GetOverHealthAttribute())
	{
		SetOverHealth(FMath::Clamp(GetOverHealth(), 0.0f, UE_MAX_FLT));
	}

	// OverArmor
	else if (Data.EvaluatedData.Attribute == GetOverArmorAttribute())
	{
		SetOverArmor(FMath::Clamp(GetOverArmor(), 0.0f, UE_MAX_FLT));
	}

	else if (Data.EvaluatedData.Attribute == GetMoveSpeedScaleAttribute())
	{
		if (IsValid(TargetCharacter))
		{
			/// TODO: Change this to something more async / in the GAS flow
			TargetCharacter->OnMoveSpeedScaleChanged(Cast<UAbilitySystemComponentBase>(SourceASC), GetMoveSpeedScale());
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/// MARK: - Attributes
//---------------------------------------------------------------------------------------------------------------------

void ULifeAttributeSet::AdjustAttributeOnMaxChange(
   FGameplayAttributeData& AffectedAttribute, 
   const FGameplayAttributeData& MaxAttribute, 
   float NewMaxValue, 
   const FGameplayAttribute& AffectedAttributeProperty
)
{
   auto ASC = GetOwningAbilitySystemComponent();
	const auto CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && ASC)
	{
		// Change current value to maintain the current Val / Max percent
		const auto CurrentValue = AffectedAttribute.GetCurrentValue();
		auto NewDelta = (CurrentMaxValue > 0.f) 
                      ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue 
                      : NewMaxValue;

		ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void ULifeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Health
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxHealth,  COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Health,     COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxShields, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Shields,    COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxArmor,   COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Armor,      COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, OverHealth, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, OverArmor,  COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MoveSpeed,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Level,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, XP,         COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, RewardXP,   COND_None, REPNOTIFY_Always);

	// Scales
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, DamageDeltScale,             COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, DamageReceivedScale,         COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, HealingDeltScale,            COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, HealingReceivedScale,        COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, KnockbackDeltScale,          COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, KnockbackReceivedScale,      COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, WeaponSpreadScale,           COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, ProjectileSpeedScale,        COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, HitscanFalloffDistanceScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, FireRateScale,               COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, CooldownRateScale,           COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MoveSpeedScale,              COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, UltimateGenRateScale,        COND_None, REPNOTIFY_Always);
}

/// HEALTH:

void ULifeAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
   // Used by the prediction system
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxHealth, OldMaxHealth);
}

void ULifeAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Health, OldHealth);
}

void ULifeAttributeSet::OnRep_MaxShields(const FGameplayAttributeData& OldMaxShields)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxShields, OldMaxShields);
}

void ULifeAttributeSet::OnRep_Shields(const FGameplayAttributeData& OldShields)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Shields, OldShields);
}

void ULifeAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxArmor, OldMaxArmor);
}

void ULifeAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Armor, OldArmor);
}

void ULifeAttributeSet::OnRep_OverHealth(const FGameplayAttributeData& OldOverHealth)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, OverHealth, OldOverHealth);
}

void ULifeAttributeSet::OnRep_OverArmor(const FGameplayAttributeData& OldOverArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, OverArmor, OldOverArmor);
}

void ULifeAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MoveSpeed, OldMoveSpeed);
}

void ULifeAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Level, OldLevel);
}

void ULifeAttributeSet::OnRep_XP(const FGameplayAttributeData& OldXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, XP, OldXP);
}

void ULifeAttributeSet::OnRep_RewardXP(const FGameplayAttributeData& OldRewardXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, RewardXP, OldRewardXP);
}


/// SCALES: 1 is normal. 0 is off. 

void ULifeAttributeSet::OnRep_DamageDeltScale(const FGameplayAttributeData& OldDamageDeltScale)
{
   // Used by the prediction system
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, DamageDeltScale, OldDamageDeltScale);
}

void ULifeAttributeSet::OnRep_DamageReceivedScale(const FGameplayAttributeData& OldDamageReceivedScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, DamageReceivedScale, OldDamageReceivedScale);
}

void ULifeAttributeSet::OnRep_HealingDeltScale(const FGameplayAttributeData& OldHealingDeltScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, HealingDeltScale, OldHealingDeltScale);
}

void ULifeAttributeSet::OnRep_HealingReceivedScale(const FGameplayAttributeData& OldHealingReceivedScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, HealingReceivedScale, OldHealingReceivedScale);
}

void ULifeAttributeSet::OnRep_KnockbackDeltScale(const FGameplayAttributeData& OldKnockbackDeltScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, KnockbackDeltScale, OldKnockbackDeltScale);
}

void ULifeAttributeSet::OnRep_KnockbackReceivedScale(const FGameplayAttributeData& OldKnockbackReceivedScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, KnockbackReceivedScale, OldKnockbackReceivedScale);
}

void ULifeAttributeSet::OnRep_WeaponSpreadScale(const FGameplayAttributeData& OldWeaponSpreadScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, WeaponSpreadScale, OldWeaponSpreadScale);
}

void ULifeAttributeSet::OnRep_ProjectileSpeedScale(const FGameplayAttributeData& OldProjectileSpeedScale)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, ProjectileSpeedScale, OldProjectileSpeedScale);
}

void ULifeAttributeSet::OnRep_HitscanFalloffDistanceScale(const FGameplayAttributeData& OldHitscanFalloffDistanceScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, HitscanFalloffDistanceScale, OldHitscanFalloffDistanceScale);
}

void ULifeAttributeSet::OnRep_FireRateScale(const FGameplayAttributeData& OldFireRateScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, FireRateScale, OldFireRateScale);
}

void ULifeAttributeSet::OnRep_CooldownRateScale(const FGameplayAttributeData& OldCooldownRateScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, CooldownRateScale, OldCooldownRateScale);
}

void ULifeAttributeSet::OnRep_MoveSpeedScale(const FGameplayAttributeData& OldMoveSpeedScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MoveSpeedScale, OldMoveSpeedScale);
}

void ULifeAttributeSet::OnRep_UltimateGenRateScale(const FGameplayAttributeData& OldUltimateGenRateScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, UltimateGenRateScale, OldUltimateGenRateScale);
}