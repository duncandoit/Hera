// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/life_attribute_set.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

ULifeAttributeSet::ULifeAttributeSet()
{

}

void ULifeAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
   
}

void ULifeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Health, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxShields, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Shields, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, MaxArmor, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, Armor, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, OverHealth, COND_None, REPNOTIFY_Always);
   DOREPLIFETIME_CONDITION_NOTIFY(ULifeAttributeSet, OverArmor, COND_None, REPNOTIFY_Always);
}

void ULifeAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData &OldMaxHealth)
{
   // Used by the prediction system
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxHealth, OldMaxHealth);
}

void ULifeAttributeSet::OnRep_Health(const FGameplayAttributeData &OldHealth)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Health, OldHealth);
}

void ULifeAttributeSet::OnRep_MaxShields(const FGameplayAttributeData &OldMaxShields)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxShields, OldMaxShields);
}

void ULifeAttributeSet::OnRep_Shields(const FGameplayAttributeData &OldShields)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Shields, OldShields);
}

void ULifeAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData &OldMaxArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, MaxArmor, OldMaxArmor);
}

void ULifeAttributeSet::OnRep_Armor(const FGameplayAttributeData &OldArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, Armor, OldArmor);
}

void ULifeAttributeSet::OnRep_OverHealth(const FGameplayAttributeData & OldOverHealth)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, OverHealth, OldOverHealth);
}

void ULifeAttributeSet::OnRep_OverArmor(const FGameplayAttributeData &OldOverArmor)
{
   GAMEPLAYATTRIBUTE_REPNOTIFY(ULifeAttributeSet, OverArmor, OldOverArmor);
}