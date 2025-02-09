// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"

// Lyra Project
#include "LyraAttributeSet.h"
#include "LyraMovementSet.generated.h"

// Unreal Engine
class ULyraCharacterMovementComponent;

/**
 * ULyraMovementSet
 *
 * Class that defines attributes that are necessary to define movement parameters.
 * For now it just covers max movement speed.
 */
UCLASS(BlueprintType)
class LYRAGAME_API ULyraMovementSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	ULyraMovementSet();
	
	ATTRIBUTE_ACCESSORS(ULyraMovementSet, MaxWalkSpeed);
	ATTRIBUTE_ACCESSORS(ULyraMovementSet, MaxSwimSpeed);
	ATTRIBUTE_ACCESSORS(ULyraMovementSet, MaxFlySpeed);
	ATTRIBUTE_ACCESSORS(ULyraMovementSet, MaxCustomMovementSpeed);
	ATTRIBUTE_ACCESSORS(ULyraMovementSet, MaxWalkSpeedCrouched);

	mutable FLyraAttributeEvent OnMaxWalkSpeed;
	mutable FLyraAttributeEvent OnMaxSwimSpeed;
	mutable FLyraAttributeEvent OnMaxFlySpeed;
	mutable FLyraAttributeEvent OnMaxCustomMovementSpeed;
	mutable FLyraAttributeEvent OnMaxWalkSpeedCrouched;

private:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxWalkSpeed;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSwimSpeed;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxFlySpeed;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxCustomMovementSpeed;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxWalkSpeedCrouched;

	UPROPERTY(Transient, Meta = (HideFromModifiers = true))
	ULyraCharacterMovementComponent* CharacterMovementComponent = nullptr;
};
