// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal Engine
#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"

// Lyra Project
#include "LyraInventoryItemInstance.generated.h"

// Unreal Engine
class FLifetimeProperty;
struct FFrame;
struct FGameplayTag;

// Lyra Project
class ULyraInventoryItemFragment;
class ULyraInventoryItemDefinition;

/**
 * @Hernan
 * FLyraInventoryWeaponAttachmentChanged
 * A message when weapon's attachment changes */
USTRUCT(BlueprintType)
struct FLyraInventoryWeaponAttachmentChanged
{
	GENERATED_BODY()

	// Item instance whose attachment list has changed
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULyraInventoryItemInstance> Instance = nullptr;
};

/**
 * @Hernan
 * FLyraInventoryWeaponAttachmentChangedWithDelta
 * A message when weapon's attachment changes
 */
USTRUCT(BlueprintType)
struct FLyraInventoryWeaponAttachmentChangedWithDelta
{
	GENERATED_BODY()

	// Item instance whose attachment list has changed
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULyraInventoryItemInstance> Instance = nullptr;

	// Attachment item instance that has changed
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<ULyraInventoryItemInstance> Attachment = nullptr;

	// Item change delta
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 AttachmentChangeDelta = 0;
};

/**
 * ULyraInventoryItemInstance
 *
 * @Hernan Changes made:
 *  - Modified to be able to be attached to another item, or receive attachments.
 * Being able to received attachments will be determined by their item definition having an
 * InventoryFragment_EquippableItem fragment.
 * Being able to be attached to another item will be determined by their item definition having an
 * InventoryFragment_AttachableItem fragment.
 *		- TObjectPtr<ULyraInventoryItemInstance> ParentItem replicated private UPROPERTY added.
 *		- TArray<TObjectPtr<ULyraInventoryItemInstance>> AttachedItems replicated private UPROPERTY added.
 *		- ULyraInventoryItemInstance* GetParentItem public BlueprintCallable function added.
 *		- void SetParentItem public function added.
 *		- TArray<ULyraInventoryItemInstance*> GetAllAttachmentItems public BlueprintCallable function added.
 *		- void AddAttachmentItem public function added.
 *		- void RemoveAttachmentItem public function added.
 *		- void OnRep_AttachedItems protected function added.
 *		- void BroadcastChangeMessage private function added.
 *		- FLyraInventoryWeaponAttachmentChanged struct definition added.
 *		- FLyraInventoryWeaponAttachmentChangedWithDelta struct definition added.
 */
UCLASS(BlueprintType)
class ULyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	ULyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<ULyraInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const ULyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<ULyraInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	/**
	 * @Hernan
	 * Returns parent item, i.e.: the item this item is attached to.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ULyraInventoryItemInstance* GetParentItem();

	/**
	 * @Hernan
	 * Sets a parent item, i.e.: the item this item is attached to.
	 * @param InParentItem Parent item
	 */
	void SetParentItem(ULyraInventoryItemInstance* InParentItem);
	
	/**
	 * @Hernan
	 * Returns attachment items, i.e.: the items attached to this item.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<ULyraInventoryItemInstance*> GetAllAttachmentItems();
	
	/**
	 * @Hernan
	 * Adds an attachment item, i.e.: the items attached to this item
	 */
	void AddAttachmentItem(ULyraInventoryItemInstance* InAttachmentItem);

	/**
	 * @Hernan
	 * Removes an attachment item, i.e.: the items attached to this item.
	 * @param InAttachmentItem Attachment item
	 */
	void RemoveAttachmentItem(ULyraInventoryItemInstance* InAttachmentItem);
	
protected:
	/**
	 * @Hernan
	 * Replication call for AttachedItems. It will send a broadcast message though UGameplayMessageSubsystem 
	 */
	UFUNCTION()
	void OnRep_AttachedItems();

private:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	void SetItemDef(TSubclassOf<ULyraInventoryItemDefinition> InDef);

	/**
	 * @Hernan
	 * Send a message through UGameplayMessageSubsystem announcing a changed in the item attachment list
	 */
	void BroadcastChangeMessage();

private:
	friend struct FLyraInventoryList;

	// Stat tags for stat counting
	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDef;

	// @Hernan Item that this item is attached to. It will only be relevant for items with InventoryFragment_AttachableItem fragments
	UPROPERTY(Replicated)
	TObjectPtr<ULyraInventoryItemInstance> ParentItem;

	// @Hernan Items that are attached to this item. It will only be relevant for items with InventoryFragment_EquippableItem fragments
	UPROPERTY(ReplicatedUsing=OnRep_AttachedItems)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> AttachedItems;
};
