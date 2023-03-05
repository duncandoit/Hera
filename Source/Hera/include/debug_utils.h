// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "debug_utils.generated.h"

/**
 * 
 */
UCLASS()
class HERA_API UHeraUtil  : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category=Utils)
	static void DebugPrint(const FString& text, FColor color)
	{
		#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1, 				// Message ID
				20.f, 			// Display duration (seconds)
				color, 			// Text color
				text, 			// Debug message
				true, 			// New displays on top
				FVector2D(1, 1)	// Text scale x, y
			);
		}
		#endif
	}
};
