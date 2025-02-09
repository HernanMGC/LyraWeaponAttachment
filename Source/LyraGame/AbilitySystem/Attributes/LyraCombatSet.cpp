// Copyright Epic Games, Inc. All Rights Reserved.

// Class
#include "LyraCombatSet.h"

// Unreal Engine
#include "Net/UnrealNetwork.h"

// Lyra Project
#include "AbilitySystem/Attributes/LyraAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraCombatSet)

// Unreal Engine
class FLifetimeProperty;

ULyraCombatSet::ULyraCombatSet()
	: BaseDamage(0.0f)
	// @Hernan - BaseDamageMultiplier initialization 
	, BaseDamageMultiplier(1.0f)
	, BaseHeal(0.0f)
{
}

void ULyraCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	// @Hernan - BaseDamageMultiplier replication setup
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, BaseDamageMultiplier, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void ULyraCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, BaseDamage, OldValue);
}

////////////////////////////////////////////////////////////////////////////////
// 	@Hernan OnRep_BaseDamageMultiplier function added
////////////////////////////////////////////////////////////////////////////////

void ULyraCombatSet::OnRep_BaseDamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, BaseDamageMultiplier, OldValue);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, BaseHeal, OldValue);
}

