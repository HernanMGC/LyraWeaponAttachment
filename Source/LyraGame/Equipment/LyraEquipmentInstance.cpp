// Copyright Epic Games, Inc. All Rights Reserved.

// Class
#include "LyraEquipmentInstance.h"

// Unreal Engine
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif // UE_WITH_IRIS
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

// Lyra Project
#include "LyraEquipmentDefinition.h"
#include "Inventory/InventoryFragment_AttachableItem.h"
#include "Inventory/LyraInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraEquipmentInstance)

// Unreal Engine
class FLifetimeProperty;
class UClass;
class USceneComponent;

ULyraEquipmentInstance::ULyraEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* ULyraEquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

void ULyraEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

#if UE_WITH_IRIS
void ULyraEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	using namespace UE::Net;

	// Build descriptors and allocate PropertyReplicationFragments for this object
	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif // UE_WITH_IRIS

APawn* ULyraEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* ULyraEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void ULyraEquipmentInstance::SpawnEquipmentActors(const TArray<FLyraEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FLyraEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void ULyraEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void ULyraEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void ULyraEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - ActivateAttachments function added
////////////////////////////////////////////////////////////////////////////////

void ULyraEquipmentInstance::ActivateAttachments() const
{
	if (Instigator != nullptr)
	{
		if (ULyraInventoryItemInstance* slotItem = Cast<ULyraInventoryItemInstance>(Instigator))
		{
			for (ULyraInventoryItemInstance* attachmentItem : slotItem->GetAllAttachmentItems())
			{
				if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
				{
					if (const UInventoryFragment_AttachableItem* attachmentInfo = attachmentItem->FindFragmentByClass<UInventoryFragment_AttachableItem>())
					{
						if (const ULyraAttachmentDefinition* attachmentDefinition = GetDefault<ULyraAttachmentDefinition>(attachmentInfo->AttachmentDefinition))
						{
							for (TSubclassOf<UGameplayEffect> gameplayEffect : attachmentDefinition->GameplayEffectsToApply)
							{
								const UGameplayEffect* gameplayEffectCDO = gameplayEffect->GetDefaultObject<UGameplayEffect>();

								ASC->ApplyGameplayEffectToSelf(gameplayEffectCDO, 0, ASC->MakeEffectContext());
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - ActivateAddedAttachment added
////////////////////////////////////////////////////////////////////////////////

void ULyraEquipmentInstance::ActivateAddedAttachment(ULyraInventoryItemInstance* AttachmentItem) const
{
	if (Instigator != nullptr)
	{
		if (ULyraInventoryItemInstance* slotItem = Cast<ULyraInventoryItemInstance>(Instigator))
		{
			if (slotItem->GetAllAttachmentItems().Contains(AttachmentItem))
			{
				if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
				{
					if (const UInventoryFragment_AttachableItem* attachmentInfo = AttachmentItem->FindFragmentByClass<UInventoryFragment_AttachableItem>())
					{
						if (const ULyraAttachmentDefinition* attachmentDefinition = GetDefault<ULyraAttachmentDefinition>(attachmentInfo->AttachmentDefinition))
						{
							for (TSubclassOf<UGameplayEffect> gameplayEffect : attachmentDefinition->GameplayEffectsToApply)
							{
								const UGameplayEffect* gameplayEffectCDO = gameplayEffect->GetDefaultObject<UGameplayEffect>();

								ASC->ApplyGameplayEffectToSelf(gameplayEffectCDO, 0, ASC->MakeEffectContext());
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - DeactivateAttachments added
////////////////////////////////////////////////////////////////////////////////

void ULyraEquipmentInstance::DeactivateAttachments() const
{
	if (ULyraInventoryItemInstance* slotItem = Cast<ULyraInventoryItemInstance>(Instigator))
	{
		for (ULyraInventoryItemInstance* attachmentItem : slotItem->GetAllAttachmentItems())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
			{
				if (const UInventoryFragment_AttachableItem* attachmentInfo = attachmentItem->FindFragmentByClass<UInventoryFragment_AttachableItem>())
				{
					if (const ULyraAttachmentDefinition* attachmentDefinition = GetDefault<ULyraAttachmentDefinition>(attachmentInfo->AttachmentDefinition))
					{
						for (TSubclassOf<UGameplayEffect> gameplayEffect : attachmentDefinition->GameplayEffectsToApply)
						{
							ASC->RemoveActiveGameplayEffectBySourceEffect(gameplayEffect, ASC);
						}
					}
				}
			}
		}	
	}
}

////////////////////////////////////////////////////////////////////////////////
// @Hernan - DeactivateRemovedAttachment added
////////////////////////////////////////////////////////////////////////////////

void ULyraEquipmentInstance::DeactivateRemovedAttachment(ULyraInventoryItemInstance* AttachmentItem) const
{
	if (ULyraInventoryItemInstance* slotItem = Cast<ULyraInventoryItemInstance>(Instigator))
	{
		// This is a special case, item has already been removed
		// as this is a reaction for attachment removal and thus it should not be contained
		if (!slotItem->GetAllAttachmentItems().Contains(AttachmentItem))
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
			{
				if (const UInventoryFragment_AttachableItem* attachmentInfo = AttachmentItem->FindFragmentByClass<UInventoryFragment_AttachableItem>())
				{
					if (const ULyraAttachmentDefinition* attachmentDefinition = GetDefault<ULyraAttachmentDefinition>(attachmentInfo->AttachmentDefinition))
					{
						for (TSubclassOf<UGameplayEffect> gameplayEffect : attachmentDefinition->GameplayEffectsToApply)
						{
							ASC->RemoveActiveGameplayEffectBySourceEffect(gameplayEffect, ASC);
						}
					}
				}
			}
		}	
	}
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void ULyraEquipmentInstance::OnRep_Instigator()
{
}

