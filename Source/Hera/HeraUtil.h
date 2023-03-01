// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "HeraUtil.generated.h"

/**
 * 
 */
UCLASS()
class HERA_API UHeraUtil  : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category=Utils)
	static void DebugPrint(const FString& text)
	{
	#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage
			(
				-1, 				// Message ID
				20.f, 			// Display duration (seconds)
				FColor::Purple, 	// Text color
				text, 			// Debug message
				true, 			// New displays on top
				FVector2D(1, 1)	// Text scale x, y
			);
		}
	#endif
	}
};
