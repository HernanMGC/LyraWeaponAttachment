// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "UObject/Object.h"

// Lyra Project
#include "LyraAttachmentDefinition.generated.h"

// Unreal Engine 
class UGameplayEffect;

/**
 * Template for the weapon attachment. It defines the gameplay effect to be applied on equipment.
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class LYRAGAME_API ULyraAttachmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor
	ULyraAttachmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Gameplay effects to apply when the weapon the attachment is attached to is equipped
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToApply;
};
