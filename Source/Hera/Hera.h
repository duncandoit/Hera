// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
   None,
   Primary1,
   Primary2,
   Utility1,
   Utility2,
   Ultimate,
   Melee,
   Jump,
   Crouch,
   Confirm,
   Cancel
};