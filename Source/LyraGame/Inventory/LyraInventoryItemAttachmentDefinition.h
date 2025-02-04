// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "UObject/Object.h"

// Lyra Project
#include "LyraInventoryItemAttachmentDefinition.generated.h"

// Unreal Engine
class UGameplayEffect;

/**
 * [NEW FOR TECH TEST]
 * ULyraInventoryItemAttachmentDefinition
 *
 * It defines an attachment definition that will be linked to an inventory item and will determine which
 * gameplay effects to add (remove9 to the pawn on item equip (unequip)
 */
UCLASS(Blueprintable, Const, Abstract)
class LYRAGAME_API ULyraInventoryItemAttachmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	// Nice name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	// List of gameplay effects to apply on weapon equip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;

public:
	// Constructor
	ULyraInventoryItemAttachmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
