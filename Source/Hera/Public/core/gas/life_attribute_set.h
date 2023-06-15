// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "life_attribute_set.generated.h"

///The Getter returns the CurrentValue
// The Setter sets the BaseValue
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/// AttributeSet governing all stats related to having, losing, and gaining health.
///
/// BaseValue changes come from Instant and Periodic GameplayEffects. 
/// CurrentValue changes come from Duration and Infinite GameplayEffects. 
/// 
///| Duration Type       | GameplayCue  | When to use
///---------------------------------------------------------------------------------------------------------------
///| Instant & Periodic  | Execute      | Permanent changes to BaseValue. GameplayTags will not be applied.
///| Duration & Infinite | Add & Remove | Temporary changes to CurrentValue and to apply GameplayTags that will be 
///                                       removed when the GameplayEffect expires or is manually removed.
UCLASS()
class HERA_API ULifeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ULifeAttributeSet();

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - UAttributeSet overrides
	//------------------------------------------------------------------------------------------------------------------

	/// Responds to changes to an Attribute's CurrentValue before the change happens.
	/// Do not use it for gameplay events and instead use it mainly for clamping CurrentValue.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/// Only triggers after changes to the BaseValue of an Attribute from an Instant or Periodic GameplayEffect. 
	/// This is a valid place to clamp the changes to BaseValue and do more Attribute manipulation when they change 
	/// from a GameplayEffect.
	/// Note: Changes to Attributes have already happened, but they have not replicated back to clients yet. 
	///       Clients will only receive the update after clamping/adjusting.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Attributes
	//------------------------------------------------------------------------------------------------------------------

	/// HEALTHPOOL:

	/// Damage is a meta attribute used by the DamageExecution to calculate final damage.
	/// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Damage)

	/// Healing is a meta attribute used to calculate final healing.
	/// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base")
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Healing)

	// Max Health
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxHealth);

	// Health. The standard HP. Does not regenerate. Must be > 0 to be considered "alive".
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Health);

	// Max Shields
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_MaxShields)
	FGameplayAttributeData MaxShields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxShields);

	// Shields. Is damaged the same way Health is but regenerates slowly.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_Shields)
	FGameplayAttributeData Shields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Shields);

	// Max Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_MaxArmor)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxArmor);

	// Armor. Takes less damage than Health or Shields. Does not regenerate. 
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Armor);

	// Over Health behaves like Health but cannot be healed.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_OverHealth)
	FGameplayAttributeData OverHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverHealth);

	// Over Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_OverArmor)
	FGameplayAttributeData OverArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverArmor);

	/// TODO: Ultimate aatributes
	//  - UltimateChargeMax
	//  - UltimateCharge
	//  - UltimateOutOfCombatDecayRate?


	/// TODO: Implement granular stats -- Should these be in another UAttributeSet?
	//
	//  BaseStat: 
	//  The fundamental, unchangeable set of stats that are unique for each Hero type or Actor type.
	//  Values for each stat range from 30 - 150.
	//                    
	//  EffortStat: 
	//  Each player has a pool of up to 200 points to put in any of the individual stats. But once 200
	//  point have been allocated collectively across all of the stats you cannot allocate any more.
	//  Values for each stat range from 0 - 200
	//
	/// STATS:
	//  - HealthStat
	//  - ShieldsStat
	//  - ArmorStat
	//  - AttackStat
	//  - DefenseStat
	//  - HealingStat
	//  - MeleeDamageStat
	//  - MeleeKnockbackStat
	
	/// SCALES: 1 is normal, >1 is a buff, <1 is a nerf, 0 is disabled

	// DamageDeltScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_DamageDeltScale)
	FGameplayAttributeData DamageDeltScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, DamageDeltScale);

	// DamageReceivedScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_DamageReceivedScale)
	FGameplayAttributeData DamageReceivedScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, DamageReceivedScale);

	// HealingDeltScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_HealingDeltScale)
	FGameplayAttributeData HealingDeltScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, HealingDeltScale);

	// HealingReceivedScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_HealingReceivedScale)
	FGameplayAttributeData HealingReceivedScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, HealingReceivedScale);

	// KnockbackDeltScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_KnockbackDeltScale)
	FGameplayAttributeData KnockbackDeltScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, KnockbackDeltScale);

	// KnockbackReceivedScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_KnockbackReceivedScale)
	FGameplayAttributeData KnockbackReceivedScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, KnockbackReceivedScale);
	
	// WeaponSpreadScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_WeaponSpreadScale)
	FGameplayAttributeData WeaponSpreadScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, WeaponSpreadScale);

	// ProjectileSpeedScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_ProjectileSpeedScale)
	FGameplayAttributeData ProjectileSpeedScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, ProjectileSpeedScale);

	// HitscanFalloffDistanceScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_HitscanFalloffDistanceScale)
	FGameplayAttributeData HitscanFalloffDistanceScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, HitscanFalloffDistanceScale);

	// FireRateScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_FireRateScale)
	FGameplayAttributeData FireRateScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, FireRateScale);

	// CooldownRateScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_CooldownRateScale)
	FGameplayAttributeData CooldownRateScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, CooldownRateScale);

	// MoveSpeedScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_MoveSpeedScale)
	FGameplayAttributeData MoveSpeedScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MoveSpeedScale);

	// UltimateGenRateScale
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base", ReplicatedUsing=OnRep_UltimateGenRateScale)
	FGameplayAttributeData UltimateGenRateScale;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, UltimateGenRateScale);
  
	/// OTHER:
	//  Values based on distance, speed, time, etc.
	//  - WeaponFalloffNearDistance
	//  - WeaponFalloffFarDistance
	//  - WeaponSpreadMin
	//  - WeaponSpreadMax
	//  - WeaponSpreadProcess
	//  - WeaponSpreadRecovery
	//  - WeaponReloadTime
	//  - WeaponAmmoMax
	//  - WeaponAmmoRemaining
	//  - ProjectileSpeed
	
	//  - FireRate

	/// MoveSpeed affects how fast characters can move in cm/s.
	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|Level", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Level)

	/// Experience points gained. Used to level up.
	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|Level", ReplicatedUsing = OnRep_XP)
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, XP)

	/// Amount of XP with which you reward your killer.
	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|Level", ReplicatedUsing = OnRep_RewardXP)
	FGameplayAttributeData RewardXP;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, RewardXP)

protected:
	// FGameplayTag HeadShotTag;

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxShields(const FGameplayAttributeData& OldMaxShields);

	UFUNCTION()
	virtual void OnRep_Shields(const FGameplayAttributeData& OldShields);

	UFUNCTION()
	virtual void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_OverHealth(const FGameplayAttributeData& OldOverHealth);

	UFUNCTION()
	virtual void OnRep_OverArmor(const FGameplayAttributeData& OldOverArmor);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	virtual void OnRep_XP(const FGameplayAttributeData& OldXP);

	UFUNCTION()
	virtual void OnRep_RewardXP(const FGameplayAttributeData& OldRewardXP);


	UFUNCTION()
	virtual void OnRep_DamageDeltScale(const FGameplayAttributeData& OldDamageDeltScale);

	UFUNCTION()
	virtual void OnRep_DamageReceivedScale(const FGameplayAttributeData& OldDamageReceivedScale);

	UFUNCTION()
	virtual void OnRep_HealingDeltScale(const FGameplayAttributeData& OldHealingDeltScale);

	UFUNCTION()
	virtual void OnRep_HealingReceivedScale(const FGameplayAttributeData& OldHealingReceivedScale);

	UFUNCTION()
	virtual void OnRep_KnockbackDeltScale(const FGameplayAttributeData& OldKnockbackDeltScale);

	UFUNCTION()
	virtual void OnRep_KnockbackReceivedScale(const FGameplayAttributeData& OldKnockbackReceivedScale);

	UFUNCTION()
	virtual void OnRep_WeaponSpreadScale(const FGameplayAttributeData& OldWeaponSpreadScale);

	UFUNCTION()
	virtual void OnRep_ProjectileSpeedScale(const FGameplayAttributeData& OldProjectileSpeedScale);

	UFUNCTION()
	virtual void OnRep_HitscanFalloffDistanceScale(const FGameplayAttributeData& OldHitscanFalloffDistanceScale);

	UFUNCTION()
	virtual void OnRep_FireRateScale(const FGameplayAttributeData& OldFireRateScale);

	UFUNCTION()
	virtual void OnRep_CooldownRateScale(const FGameplayAttributeData& OldCooldownRateScale);

	UFUNCTION()
	virtual void OnRep_MoveSpeedScale(const FGameplayAttributeData& OldMoveSpeedScale);

	UFUNCTION()
	virtual void OnRep_UltimateGenRateScale(const FGameplayAttributeData& OldUltimateGenRateScale);

	/// Proportionally adjust the value of an attribute when it's associated max attribute changes.
	void AdjustAttributeOnMaxChange(
		FGameplayAttributeData& AffectedAttribute, 
		const FGameplayAttributeData& MaxAttribute, 
		float NewMaxValue, 
		const FGameplayAttribute& AffectedAttributeProperty
	);

private:
	/// Apply the damage to all health components in this order:
	/// OverArmor > OverHealth > Armor > Shield > Health.
	void HandleDamage(const float DamageReceived);

	/// Apply the healing to healable health components in this order:
	/// Health > Shields > Armor.
	void HandleHealing(const float HealingReceived);

	/// Grant the Source ASC rewards for defeating you.
	void HandleKillReward(UAbilitySystemComponent* SourceASC);
};
