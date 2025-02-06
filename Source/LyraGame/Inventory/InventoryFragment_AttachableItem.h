﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// Lyra Project
#include "LyraInventoryItemDefinition.h"
#include "Equipment/LyraAttachmentDefinition.h"
#include "InventoryFragment_AttachableItem.generated.h"


/**
 * Inventory fragment that adds weapon attachment functionality to an inventory item. This fragment should not coexist
 * with UInventoryFragment_EquippableItem.
 */
UCLASS()
class LYRAGAME_API UInventoryFragment_AttachableItem : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	// Attachment definition
	UPROPERTY(EditAnywhere, Category=Lyra)
	TSubclassOf<ULyraAttachmentDefinition> AttachmentDefinition;
};
