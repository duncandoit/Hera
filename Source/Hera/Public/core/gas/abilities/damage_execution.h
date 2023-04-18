// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "damage_execution.generated.h"

/// NOTES:
// ExecutionCalculations can only be used with Instant and Periodic GameplayEffects and they can not be predicted.
// For Local Predicted, Server Only, and Server Initiated GameplayAbilities, the Execution only calls on the Server.
// Order of operations: ExecutionCalculation > PreAttributeChange > PostGameplayEffectExecute
// They only manipulate Attributes' BaseValues

/// In Blueprints - add to a GameplayEffect's Executions > Calculation Class inside the details panel.
UCLASS()
class HERA_API UDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UDamageExecution();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;

protected:
	float CritMultiplier;
};
