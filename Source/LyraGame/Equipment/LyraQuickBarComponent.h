// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal Engine
#include "Components/ControllerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// Lyra Project
#include "Inventory/LyraInventoryItemInstance.h"
#include "LyraQuickBarComponent.generated.h"

// Unreal Engine
class AActor;
class UObject;
struct FFrame;

// Lyra Project
class ULyraEquipmentInstance;
class ULyraEquipmentManagerComponent;
struct FLyraNotificationMessage;

/**
 * @Hernan Changes made:
 *	- BeginPlay override modified to register a UGameplayMessageSubsystem listener for changes on weapon attachment
 *	changes
 *	- EndPlay override added to deregister the UGameplayMessageSubsystem listener
 *	- OnWeaponAttachmentChangedWithDelta function added.
 *	- FGameplayMessageListenerHandle WeaponAttachmentChangedWithDeltaListenerHandler private property added
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class ULyraQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	ULyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="Lyra")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Lyra")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	TArray<ULyraInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	ULyraInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, ULyraInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	ULyraInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	// Overriden to: Add defaulted slots and register message listener
	virtual void BeginPlay() override;

	// Overriden to: Deregister message listener
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	ULyraEquipmentManagerComponent* FindEquipmentManager() const;

	/**
	 * Reacts to WeaponAttachmentChangedWithDelta events to add or remove the attachment effect if the parent item is
	 * currently equipped.
	 * @param Channel Channel listened to
	 * @param Notification An FLyraInventoryWeaponAttachmentChangedWithDelta used to determine if Attachement needs to
	 * be added or removed
	 */
	void OnWeaponAttachmentChangedWithDelta(FGameplayTag Channel, const FLyraInventoryWeaponAttachmentChangedWithDelta& Notification);
	
protected:
	UPROPERTY()
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<ULyraEquipmentInstance> EquippedItem;

	// Message listener handle for WeaponAttachmentChangedWithDelta
	FGameplayMessageListenerHandle WeaponAttachmentChangedWithDeltaListenerHandler;
};


USTRUCT(BlueprintType)
struct FLyraQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<ULyraInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FLyraQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};
