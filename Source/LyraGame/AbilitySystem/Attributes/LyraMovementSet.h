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
 * @Hernan
 * ULyraMovementSet
 *
 * Class that defines attributes that are necessary to define movement parameters.
 * For now, it just covers max movement speed.
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
	// Attribute that will replace ULyraMovementComponentMaxWalkSpeed in ULyraMovementComponent::GetMaxSpeed() 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxWalkSpeed;
	
	// Attribute that will replace ULyraMovementComponentMaxSwimSpeed in ULyraMovementComponent::GetMaxSpeed() 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSwimSpeed;
	
	// Attribute that will replace ULyraMovementComponentMaxFlySpeed in ULyraMovementComponent::GetMaxSpeed() 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxFlySpeed;
	
	// Attribute that will replace ULyraMovementComponentMaxCustomMovementSpeed in ULyraMovementComponent::GetMaxSpeed() 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxCustomMovementSpeed;

	// Attribute that will replace ULyraMovementComponentMaxWalkSpeedCrouched in ULyraMovementComponent::GetMaxSpeed() 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lyra|MovementSpeed", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxWalkSpeedCrouched;
};
