// Copyright Epic Games, Inc. All Rights Reserved.

// Class
#include "LyraInventoryItemInstance.h"

// Unreal Engine
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

// Lyra Project
#include "Inventory/LyraInventoryItemDefinition.h"
#include "LyraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryItemInstance)

// Unreal Engine
class FLifetimeProperty;

ULyraInventoryItemInstance::ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULyraInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
	// @Hernan - ParentItem replication setup
	DOREPLIFETIME(ThisClass, ParentItem);
	// @Hernan - AttachedItems replication setup
	DOREPLIFETIME(ThisClass, AttachedItems);
}

#if UE_WITH_IRIS
void ULyraInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

void ULyraInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void ULyraInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 ULyraInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool ULyraInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void ULyraInventoryItemInstance::SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - GetParentItem function added
////////////////////////////////////////////////////////////////////////////////

ULyraInventoryItemInstance* ULyraInventoryItemInstance::GetParentItem()
{
	return ParentItem;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - SetParentItem function added
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::SetParentItem(ULyraInventoryItemInstance* InParentItem)
{
	ParentItem = InParentItem;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - GetAllAttachmentItems function added
////////////////////////////////////////////////////////////////////////////////

TArray<ULyraInventoryItemInstance*> ULyraInventoryItemInstance::GetAllAttachmentItems()
{
	return AttachedItems;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - AddAttachmentItem function added
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::AddAttachmentItem(ULyraInventoryItemInstance* InAttachmentItem)
{
	if (AttachedItems.Num() >= GetStatTagStackCount(FGameplayTag::RequestGameplayTag("Lyra.ShooterGame.Weapon.AttachmentSlots", false)))
	{
		return;
	}

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
// @Hernan - RemoveAttachmentItem function added
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
// @Hernan - OnRep_AttachedItems function added
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryItemInstance::OnRep_AttachedItems()
{
	BroadcastChangeMessage();
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - BroadcastChangeMessage function added
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
