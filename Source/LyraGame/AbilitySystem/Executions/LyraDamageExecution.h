// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal Engine
#include "GameplayEffectExecutionCalculation.h"

// Lyra Project
#include "LyraDamageExecution.generated.h"

// Unreal Engine
class UObject;


/**
 * ULyraDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 *
 *	@Hernan Changes made:
 *	 - BaseDamageMultiplier replicated private UPROPERTY has been added to handle damage increases.
 *	 - BaseDamageMultiplier added to DamageDone formula.
 */
UCLASS()
class ULyraDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	ULyraDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
