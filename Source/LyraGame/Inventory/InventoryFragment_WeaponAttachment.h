// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// Lyra Project
#include "LyraInventoryItemDefinition.h"
#include "InventoryFragment_WeaponAttachment.generated.h"

// Lyra Project
class ULyraInventoryItemAttachmentDefinition;

/**
 * [NEW FOR TECH TEST]
 *
 * UInventoryFragment_WeaponAttachment
 *
 * Fragment to set initial weapon attachment
 */
UCLASS()
class LYRAGAME_API UInventoryFragment_WeaponAttachment : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TSubclassOf<ULyraInventoryItemAttachmentDefinition>> InitialWeaponAttachments;
	
public:
	virtual void OnInstanceCreated(ULyraInventoryItemInstance* Instance) const override;

	TArray<TSubclassOf<ULyraInventoryItemAttachmentDefinition>> GetInitialWeaponAttachments() const;
};
