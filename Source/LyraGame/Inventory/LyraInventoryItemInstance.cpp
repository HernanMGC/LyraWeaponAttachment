// Copyright Epic Games, Inc. All Rights Reserved.

// Class
#include "LyraInventoryItemInstance.h"

// Unreal Engine
#include "Net/UnrealNetwork.h"
#include "NativeGameplayTags.h"
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS
#include "GameFramework/GameplayMessageSubsystem.h"

// Lyra Project
#include "LyraGameplayTags.h"
#include "Inventory/LyraInventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryItemInstance)

// Unreal Engine
class FLifetimeProperty;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

ULyraInventoryItemInstance::ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, ParentItem);
	DOREPLIFETIME(ThisClass, AttachedItems);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#if UE_WITH_IRIS
void ULyraInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 ULyraInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

bool ULyraInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

ULyraInventoryItemInstance* ULyraInventoryItemInstance::GetParentItem()
{
	return ParentItem;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::SetParentItem(ULyraInventoryItemInstance* InParentItem)
{
	ParentItem = InParentItem;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

TArray<ULyraInventoryItemInstance*> ULyraInventoryItemInstance::GetAllAttachmentItems()
{
	return AttachedItems;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::AddAttachmentItem(ULyraInventoryItemInstance* InAttachmentItem)
{
	AttachedItems.AddUnique(InAttachmentItem);
	
	FLyraInventoryWeaponAttachmentChangedWithDelta Message;
	Message.Instance = this;
	Message.Attachment = InAttachmentItem;
	Message.AttachmentChangeDelta = 1;
	
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_WeaponAttachmentChangedWithDelta, Message);
	OnRep_AttachedItems();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::RemoveAttachmentItem(ULyraInventoryItemInstance* InAttachmentItem)
{
	AttachedItems.Remove(InAttachmentItem);
	
	FLyraInventoryWeaponAttachmentChangedWithDelta Message;
	Message.Instance = this;
	Message.Attachment = InAttachmentItem;
	Message.AttachmentChangeDelta = -1;
	
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_WeaponAttachmentChangedWithDelta, Message);
	OnRep_AttachedItems();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::OnRep_AttachedItems()
{
	BroadcastChangeMessage();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::BroadcastChangeMessage()
{
	FLyraInventoryWeaponAttachmentChanged Message;
	Message.Instance = this;
	
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(LyraGameplayTags::TAG_Lyra_Inventory_Message_WeaponAttachmentChanged, Message);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

const ULyraInventoryItemFragment* ULyraInventoryItemInstance::FindFragmentByClass(TSubclassOf<ULyraInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<ULyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
