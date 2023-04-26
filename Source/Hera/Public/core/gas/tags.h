// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

namespace HeraTags
{
   /// EFFECTS:
   const FGameplayTag Tag_Damage = FGameplayTag::RequestGameplayTag(FName("Effect.Damage"));
   const FGameplayTag Tag_Healing = FGameplayTag::RequestGameplayTag(FName("Effect.Healing"));
   const FGameplayTag Tag_Buff = FGameplayTag::RequestGameplayTag(FName("Effect.Buff"));
   const FGameplayTag Tag_Debuff = FGameplayTag::RequestGameplayTag(FName("Effect.Debuff"));

   const FGameplayTag Tag_MeleeAttack = FGameplayTag::RequestGameplayTag(FName("Attack.Melee"));
   const FGameplayTag Tag_BalisticAttack = FGameplayTag::RequestGameplayTag(FName("Attack.Balistic"));
   const FGameplayTag Tag_BeamAttack = FGameplayTag::RequestGameplayTag(FName("Attack.Beam"));
   const FGameplayTag Tag_MagicAttack = FGameplayTag::RequestGameplayTag(FName("Attack.Magic"));
   const FGameplayTag Tag_EnergyAttack = FGameplayTag::RequestGameplayTag(FName("Attack.Energy"));

   /// CHARACTER:
   const FGameplayTag Tag_Landed = FGameplayTag::RequestGameplayTag(FName("Character.State.Landed"));
}