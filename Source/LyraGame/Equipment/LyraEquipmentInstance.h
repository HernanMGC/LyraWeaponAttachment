// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Unreal Engine
#include "Engine/World.h"

// Lyra Project
#include "Inventory/LyraInventoryItemInstance.h"
#include "LyraEquipmentInstance.generated.h"

// Unreal Engine
class AActor;
class APawn;
struct FFrame;

// Lyra Project
struct FLyraEquipmentActorToSpawn;

/**
 * @Hernan
 * FSpawnedActorsPerAttachment
 *
 * Struct that serves a roundabout for replicated TMaps that stores the Spawn Actors for each Attachment attached to the
 * weapon.
 */
USTRUCT()
struct FSpawnedActorsPerAttachment
{
	GENERATED_BODY()

	// Attachment Item
	UPROPERTY()
	TObjectPtr<ULyraInventoryItemInstance> Attachment;

	// Spawned actors for Attachment
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedAttachmentActors;
};

/**
 * ULyraEquipmentInstance
 *
 * A piece of equipment spawned and applied to a pawn
 *
 * @Hernan Changes made:
 *  - SpawnAttachmentActors function added.
 *  - DestroyAttachmentActors function added.
 *  - DestroyAttachmentActors function added.
 *  - ActivateAttachments function added.
 *  - ActivateAddedAttachment function added.
 *  - DestroyAttachmentActors called add inside DestroyEquipmentActors to ensure that no Spawn Actor is left visible
 *    when weapon is unequipped.
 */
UCLASS(BlueprintType, Blueprintable)
class ULyraEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	ULyraEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category=Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnEquipmentActors(const TArray<FLyraEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	/**
	 * @Hernan
	 * Given an attachment item (it is assumed that it is an Item with AttachableItem fragment in it), Spawn attachment
	 * actors to the Weapon or Pawn for visual feedback purposes.
	 * @param AttachmentItem Attachment Item whose actors need to be spawn
	 * @param ActorsToSpawn Actors to spawn info
	 */
	virtual void SpawnAttachmentActors(ULyraInventoryItemInstance* AttachmentItem, const TArray<FLyraEquipmentActorToSpawn>& ActorsToSpawn);

	/**
	 * @Hernan
	 * Despawn all actors spawn for every Attachment Item attached to this weapon.
	 */
	virtual void DestroyAttachmentActors();

	/**
	 * @Hernan
	 * Despawn all actors spawn for an specific Attachment Item attached to this weapon.
	 * @param AttachmentItem Attachment Item whose actors need to be despawn 
	 */
	virtual void DestroyAttachmentActors(ULyraInventoryItemInstance* AttachmentItem);

	virtual void OnEquipped();
	virtual void OnUnequipped();

	/**
	 * @Hernan
	 * Applies the effects of all the attachment items attached to the item.
	 */
	void ActivateAttachments();

	/**
	 * @Hernan
	 * Applies the effects of a single attachment item.
	 * @param AttachmentItem Item to attach
	 */
	void ActivateAddedAttachment(ULyraInventoryItemInstance* AttachmentItem);

	/**
	 * @Hernan
	 * Removes the effects of all the attachment items attached to the item.
	 */
	void DeactivateAttachments();

	/**
	 * @Hernan
	 * Removes the effects of a single attachment item that has already been detached from the item.
	 * @param AttachmentItem Item to attach
	 */
	void DeactivateRemovedAttachment(ULyraInventoryItemInstance* AttachmentItem);

protected:
#if UE_WITH_IRIS
	/** Register all replication fragments */
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif // UE_WITH_IRIS

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:
	UFUNCTION()
	void OnRep_Instigator();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;

	// @Hernan
	// List of Actors spawned by Attachment Items
	UPROPERTY(Replicated)
	TArray<FSpawnedActorsPerAttachment> SpawnedAttachmentActors;
};
