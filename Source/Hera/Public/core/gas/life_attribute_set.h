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
UCLASS()
class HERA_API ULifeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	/// Apply the damage to all health components in this order:
	/// OverArmor > OverHealth > Armor > Shield > Health.
	void HandleDamage(const float DamageReceived);

	/// Apply the healing to healable health components in this order:
	/// Health > Shields > Armor.
	void HandleHealing(const float HealingReceived);

	/// Grant the Source ASC rewards for defeating you.
	void HandleKillReward(UAbilitySystemComponent* SourceASC);

public:
	ULifeAttributeSet();

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - UAttributeSet overrides
	//------------------------------------------------------------------------------------------------------------------

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - Health related Attributes
	//------------------------------------------------------------------------------------------------------------------

	/// Damage is a meta attribute used by the DamageExecution to calculate final damage.
	/// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|Health Pool")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Damage)

	/// Healing is a meta attribute used to calculate final healing.
	/// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Hera|Attributes|Base|Health Pool")
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Healing)

	/// Max Health
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxHealth);

	/// Health. The standard HP. Does not regenerate. Must be > 0 to be considered "alive".
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Health);

	/// Max Shields
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_MaxShields)
	FGameplayAttributeData MaxShields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxShields);

	/// Shields. Is damaged the same way Health is but regenerates slowly.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_Shields)
	FGameplayAttributeData Shields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Shields);

	/// Max Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_MaxArmor)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxArmor);

	/// Armor. Takes less damage than Health or Shields. Does not regenerate. 
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Armor);

	/// Over Health behaves like Health but cannot be healed.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_OverHealth)
	FGameplayAttributeData OverHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverHealth);

	/// Over Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Attributes|Base|Health Pool", ReplicatedUsing=OnRep_OverArmor)
	FGameplayAttributeData OverArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverArmor);

	/// TODO: Ultimate aatributes
	//  - UltimateChargeMax
	//  - UltimateCharge
	//  - UltimateOutOfCombatDecayRate?

	//------------------------------------------------------------------------------------------------------------------
	/// MARK: - BaseStat / EffortStat Attributes
	//------------------------------------------------------------------------------------------------------------------

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
	//  - RegenerateHealthPoolStat
	//  - MeleeDamageStat
	//  - MeleeKnockbackStat
	//  
	/// SCALES: 
	//  1 is normal, >1 is a buff, <1 is a nerf, 0 is disabled
	//  - DamageDeltScale
	//  - DamageReceivedScale
	//  - HealingDeltScale
	//  - HealingReceivedScale
	//  - RegenerateHealthPoolScale
	//  - WeaponSpreadScale
	//  - ProjectileSpeedScale
	//  - DamageFalloffDistanceScale
	//  - DamageMaxRangeScale
	//  - FireRateScale
	//  - CooldownRateScale
	//  - MoveSpeedScale
	//  
	/// OTHER:
	//  Values based on distance, speed, time, etc.
	//  - WeaponDamageFalloffDistance
	//  - WeaponDamageMaxDistance
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

	/// Proportionally adjust the value of an attribute when it's associated max attribute changes.
	void AdjustAttributeOnMaxChange(
		FGameplayAttributeData& AffectedAttribute, 
		const FGameplayAttributeData& MaxAttribute, 
		float NewMaxValue, 
		const FGameplayAttribute& AffectedAttributeProperty
	);
};
