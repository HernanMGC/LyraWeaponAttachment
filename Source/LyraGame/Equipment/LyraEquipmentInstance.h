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
 * ULyraEquipmentInstance
 *
 * A piece of equipment spawned and applied to a pawn
 *
 * @Hernan Changes made:
 *	- AddAttachments function added
 *	- AddAttachment function added
 *	- RemoveAttachments function added
 *	- RemoveAttachment function added
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

	virtual void OnEquipped();
	virtual void OnUnequipped();

	/**
	 * Applies the effects of all the attachment items attached to the item.
	 */
	void AddAttachments();

	/**
	 * Applies the effects of a single attachment item attached to the item.
	 */
	void AddAttachment(ULyraInventoryItemInstance* AttachmentItem);

	/**
	 * Removes the effects of all the attachment items attached to the item.
	 */
	void RemoveAttachments();

	/**
	 * Removes the effects of a single attachment item attached to the item.
	 */
	void RemoveAttachment(ULyraInventoryItemInstance* AttachmentItem);

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
};
