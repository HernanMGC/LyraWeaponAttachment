// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// Lyra Project
#include "LyraInventoryItemDefinition.h"
#include "InventoryFragment_AttachableItem.generated.h"

// Unreal Engine
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class LYRAGAME_API UInventoryFragment_AttachableItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
public:
	// List of gameplay effects to apply on weapon equip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;
};
