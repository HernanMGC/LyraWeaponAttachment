// Fill out your copyright notice in the Description page of Project Settings.

// Class
#include "LyraMovementSet.h"

#include "Character/LyraCharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
//
ULyraMovementSet::ULyraMovementSet()
	: MaxFlySpeed(0.f)
	, MaxWalkSpeed(0.f)
	, MaxSwimSpeed(0.f)
	, MaxCustomMovementSpeed(0.f)
	, MaxWalkSpeedCrouched(0.f)
{
}
void ULyraMovementSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULyraMovementSet, MaxFlySpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraMovementSet, MaxWalkSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraMovementSet, MaxSwimSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraMovementSet, MaxCustomMovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraMovementSet, MaxWalkSpeedCrouched, COND_None, REPNOTIFY_Always);
}
