// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "healing_execution.generated.h"

/// For Local Predicted, Server Only, and Server Initiated GameplayAbilities, the Execution only calls on the Server.
UCLASS()
class HERA_API UHealingExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UHealingExecution();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;

protected:
	float CritMultiplier;
};
