// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "life_attribute_set.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// AttributeSet governing all stats related to having, losing, and gaining health.
//
// Order of damaging: OverArmor, OverHealth, Armor, Shields, Health
// Order of healing:  Health, Shields, Armor
//
//
UCLASS()
class HERA_API ULifeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ULifeAttributeSet();

	// MARK: - UAttributeSet overrides

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Mark: - Health related Attributes

	// Max Health
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxHealth);

	// Health. The standard HP. Does not regenerate. Must be > 0 to be considered "alive".
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Health);

	// Max Shields
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_MaxShields)
	FGameplayAttributeData MaxShields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxShields);

	// Shields. Is damaged the same way Health is but regenerates slowly.
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_Shields)
	FGameplayAttributeData Shields;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Shields);

	// Max Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_MaxArmor)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, MaxArmor);

	// Armor. Takes less damage than Health or Shields. Does not regenerate. 
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, Armor);

	// Over Health
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_OverHealth)
	FGameplayAttributeData OverHealth;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverHealth);

	// Over Armor
	UPROPERTY(BlueprintReadOnly, Category="Hera|Life|Attributes", ReplicatedUsing=OnRep_OverArmor)
	FGameplayAttributeData OverArmor;
	ATTRIBUTE_ACCESSORS(ULifeAttributeSet, OverArmor);

	// MARK: - Stat related Attributes

protected:

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
};
