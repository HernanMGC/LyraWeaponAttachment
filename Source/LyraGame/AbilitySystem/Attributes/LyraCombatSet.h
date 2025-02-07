// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal Engine
#include "AbilitySystemComponent.h"

// Lyra Project
#include "LyraAttributeSet.h"
#include "LyraCombatSet.generated.h"

// Unreal Engine
class UObject;
struct FFrame;


/**
 * ULyraCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 *
 *	@Hernan Changes made:
 *	 - float BaseDamageMultiplier replicated private UPROPERTY has been added to handle damage increases
 */
UCLASS(BlueprintType)
class ULyraCombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	ULyraCombatSet();

	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseDamageMultiplier);
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseDamageMultiplier(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;
	
	// The base damage multiplier to apply on damage application execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamageMultiplier, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamageMultiplier;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;
};
