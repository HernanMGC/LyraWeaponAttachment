// Copyright Epic Games, Inc. All Rights Reserved.

// Class
#include "LyraInventoryManagerComponent.h"

// Unreal Engine
#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

// Lyra Project
#include "Equipment/LyraEquipmentManagerComponent.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "InventoryFragment_AttachableItem.h"
#include "InventoryFragment_EquippableItem.h"
#include "LyraInventoryItemDefinition.h"
#include "LyraInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraInventoryManagerComponent)

// Unreal Engine
class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Inventory_Message_StackChanged, "Lyra.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FLyraInventoryEntry

FString FLyraInventoryEntry::GetDebugString() const
{
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FLyraInventoryList

void FLyraInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FLyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FLyraInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FLyraInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FLyraInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FLyraInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FLyraInventoryList::BroadcastChangeMessage(FLyraInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FLyraInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Lyra_Inventory_Message_StackChanged, Message);
}

ULyraInventoryItemInstance* FLyraInventoryList::AddEntry(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	ULyraInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FLyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<ULyraInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (ULyraInventoryItemFragment* Fragment : GetDefault<ULyraInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const ULyraInventoryItemDefinition* ItemCDO = GetDefault<ULyraInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FLyraInventoryList::AddEntry(ULyraInventoryItemInstance* Instance)
{
	// @Hernan - Function now is implemented. In the base Sample Project this function was unimplemented 
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FLyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = Instance;
	NewEntry.StackCount = 1;

	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /*OldCount=*/ 0, /*NewCount=*/ NewEntry.StackCount);
}

void FLyraInventoryList::RemoveEntry(ULyraInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FLyraInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			// @Hernan - Message sent here to assure that Server in a ListenServer gets it too
			BroadcastChangeMessage(Entry, /*OldCount=*/ 1, /*NewCount=*/ 0);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<ULyraInventoryItemInstance*> FLyraInventoryList::GetAllItems() const
{
	TArray<ULyraInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FLyraInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// ULyraInventoryManagerComponent

ULyraInventoryManagerComponent::ULyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void ULyraInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool ULyraInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::AddItemDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 StackCount)
{
	ULyraInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_MoveItemInstanceFrom_Implementation function added 
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryManagerComponent::Server_MoveItemInstanceFrom_Implementation(
	ULyraInventoryItemInstance* ItemInstance, ULyraInventoryManagerComponent* SourceInventory)
{
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	if (SourceInventory == nullptr)
	{
		return;
	}

	if (SourceInventory == this)
	{
		return;
	}

	if (SourceInventory->GetAllItems().Contains(ItemInstance))
	{
		// If an attachable item is moved to another inventory it is safe to assume that its attachment connections need to be reset.
		if (const UInventoryFragment_AttachableItem* attachableItem = ItemInstance->FindFragmentByClass<UInventoryFragment_AttachableItem>())
		{
			if (ULyraInventoryItemInstance* weaponItem = ItemInstance->GetParentItem())
			{
				weaponItem->RemoveAttachmentItem(ItemInstance);
				ItemInstance->SetParentItem(nullptr);
			}
		}
		
		SourceInventory->RemoveItemInstance(ItemInstance);
		AddItemInstance(ItemInstance);	
	}
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_MoveItemInstanceFrom_Validate function added 
////////////////////////////////////////////////////////////////////////////////

bool ULyraInventoryManagerComponent::Server_MoveItemInstanceFrom_Validate(ULyraInventoryItemInstance* ItemInstance,
	ULyraInventoryManagerComponent* SourceInventory)
{
	// It may be a bit harsh for validation
	TArray<ULyraInventoryItemInstance*> sourceItems = SourceInventory->GetAllItems();
	TArray<ULyraInventoryItemInstance*> destinationItems = GetAllItems();
	return sourceItems.Contains(ItemInstance) && !destinationItems.Contains(ItemInstance);
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_MoveItemInstanceTo_Implementation function added 
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryManagerComponent::Server_MoveItemInstanceTo_Implementation(ULyraInventoryItemInstance* ItemInstance, ULyraInventoryManagerComponent* DestinationInventory)
{
	if (ItemInstance == nullptr)
	{
		return;
	}
	
	if (DestinationInventory == nullptr)
	{
		return;
	}

	if (DestinationInventory == this)
	{
		return;
	}
	
	// If an attachable item is moved to another inventory it is safe to assume that its attachment connections need to be reset.
	if (const UInventoryFragment_AttachableItem* attachableItem = ItemInstance->FindFragmentByClass<UInventoryFragment_AttachableItem>())
	{
		if (ULyraInventoryItemInstance* weaponItem = ItemInstance->GetParentItem())
		{
			weaponItem->RemoveAttachmentItem(ItemInstance);
			ItemInstance->SetParentItem(nullptr);
		}
	}
	
	RemoveItemInstance(ItemInstance);
	DestinationInventory->AddItemInstance(ItemInstance);
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_MoveItemInstanceTo_Validate function added 
////////////////////////////////////////////////////////////////////////////////

bool ULyraInventoryManagerComponent::Server_MoveItemInstanceTo_Validate(ULyraInventoryItemInstance* ItemInstance,
	ULyraInventoryManagerComponent* DestinationInventory)
{

	// It may be a bit harsh for validation
	TArray<ULyraInventoryItemInstance*> sourceItems = GetAllItems();
	TArray<ULyraInventoryItemInstance*> destinationItems = DestinationInventory->GetAllItems();
	return sourceItems.Contains(ItemInstance) && !destinationItems.Contains(ItemInstance);
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_AttachItemToWeapon_Implementation function added 
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryManagerComponent::Server_AttachItemToWeapon_Implementation(ULyraInventoryItemInstance* WeaponInstance, ULyraInventoryItemInstance* AttachmentInstance,
	ULyraInventoryManagerComponent* AttachmentSourceInventory)
{
	if (WeaponInstance == nullptr || AttachmentInstance == nullptr || AttachmentSourceInventory == nullptr)
	{
		return;
	}
	const UInventoryFragment_EquippableItem* equippableItem = WeaponInstance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
	if (equippableItem == nullptr)
	{
		return;
	}
	
	const UInventoryFragment_AttachableItem* attachableItem = AttachmentInstance->FindFragmentByClass<UInventoryFragment_AttachableItem>();
	if (attachableItem == nullptr)
	{
		return;
	}

	if (WeaponInstance->GetAllAttachmentItems().Num() >= WeaponInstance->GetStatTagStackCount(FGameplayTag::RequestGameplayTag("Lyra.ShooterGame.Weapon.AttachmentSlots", false)))
	{
		return;
	}
	
	if (this != AttachmentSourceInventory)
	{
		Server_MoveItemInstanceFrom(AttachmentInstance, AttachmentSourceInventory);
	}

	if (ULyraInventoryItemInstance* oldParentItem = AttachmentInstance->GetParentItem())
	{
			oldParentItem->RemoveAttachmentItem(AttachmentInstance);
	}
	WeaponInstance->AddAttachmentItem(AttachmentInstance);
	AttachmentInstance->SetParentItem(WeaponInstance);
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - Server_AttachItemToWeapon_Validate function added 
////////////////////////////////////////////////////////////////////////////////

bool ULyraInventoryManagerComponent::Server_AttachItemToWeapon_Validate(ULyraInventoryItemInstance* WeaponInstance, ULyraInventoryItemInstance* AttachmentInstance,
	ULyraInventoryManagerComponent* AttachmentSourceInventory)
{
	TArray<ULyraInventoryItemInstance*> sourceItems = AttachmentSourceInventory->GetAllItems();
	TArray<ULyraInventoryItemInstance*> destinationItems = GetAllItems();
	return sourceItems.Contains(AttachmentInstance) && destinationItems.Contains(WeaponInstance);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraInventoryManagerComponent::AddItemInstance(ULyraInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void ULyraInventoryManagerComponent::RemoveItemInstance(ULyraInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<ULyraInventoryItemInstance*> ULyraInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

ULyraInventoryItemInstance* ULyraInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	for (const FLyraInventoryEntry& Entry : InventoryList.Entries)
	{
		ULyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - GetInventoryStatus function added
////////////////////////////////////////////////////////////////////////////////

FText ULyraInventoryManagerComponent::GetInventoryStatus()
{
	FString statusString;

	ULyraQuickBarComponent* quickBarComponent = GetOwner()->GetComponentByClass<ULyraQuickBarComponent>();
	
	for (ULyraInventoryItemInstance* item : GetAllItems())
	{
		if (const UInventoryFragment_EquippableItem* equippableInfo = item->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			if (quickBarComponent != nullptr)
			{
				statusString += "[";
				statusString += (quickBarComponent->GetActiveSlotItem() == item) ? "X" : " ";
				statusString += "] ";	
			}
			
			const ULyraInventoryItemDefinition* equippableDef = GetDefault<ULyraInventoryItemDefinition>( item->GetItemDef());
			statusString += equippableDef->DisplayName.ToString() + ":";

			for (ULyraInventoryItemInstance* attachmentItem : item->GetAllAttachmentItems())
			{
				const ULyraInventoryItemDefinition* attachmentDef = GetDefault<ULyraInventoryItemDefinition>( attachmentItem->GetItemDef());
				statusString += " [" + attachmentDef->DisplayName.ToString() + "]";
			}
			
			statusString += "\n";
		}
	}
	
	return FText::FromString(statusString);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

int32 ULyraInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FLyraInventoryEntry& Entry : InventoryList.Entries)
	{
		ULyraInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool ULyraInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<ULyraInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (ULyraInventoryItemInstance* Instance = ULyraInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void ULyraInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing ULyraInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FLyraInventoryEntry& Entry : InventoryList.Entries)
		{
			ULyraInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool ULyraInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FLyraInventoryEntry& Entry : InventoryList.Entries)
	{
		ULyraInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class ULyraInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class ULyraInventoryFilter_HasTag : public ULyraInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(ULyraInventoryItemInstance* Instance) const { return true; }
// };


