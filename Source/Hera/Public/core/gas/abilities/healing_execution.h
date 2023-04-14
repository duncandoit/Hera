// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "healing_execution.generated.h"

/// Add to a GameplayEffect's Executions > Calculation Class
/// ExecutionCalculations can only be used with Instant and Periodic GameplayEffects and they can not be predicted.
/// For Local Predicted, Server Only, and Server Initiated GameplayAbilities, the Execution only calls on the Server.
/// Order of operations: ExecutionCalculation > PreAttributeChange > PostGameplayEffectExecute
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
