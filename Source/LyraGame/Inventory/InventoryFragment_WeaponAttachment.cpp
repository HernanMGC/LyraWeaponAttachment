// Fill out your copyright notice in the Description page of Project Settings.

// Class
#include "InventoryFragment_WeaponAttachment.h"

// Lyra Project
#include "LyraInventoryItemInstance.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void UInventoryFragment_WeaponAttachment::OnInstanceCreated(ULyraInventoryItemInstance* Instance) const
{
	for (const auto& weaponAttachmentDef : InitialWeaponAttachments)
	{
		Instance->AddWeaponAttachment(weaponAttachmentDef);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

TArray<TSubclassOf<ULyraInventoryItemAttachmentDefinition>> UInventoryFragment_WeaponAttachment::GetInitialWeaponAttachments() const
{
	return InitialWeaponAttachments;
}
