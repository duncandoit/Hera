// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
   None,
   Primary,
   Secondary,
   Tertiary,
   Quaternary,
   Ultimate,
   Melee,
   Jump,
   Crouch,
   Confirm,
   Cancel
};