// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Lyra Project
#include "Interaction/IInteractableTarget.h"
#include "LyraWorldItemContainer.generated.h"

// Lyra Project
class ULyraInventoryItemInstance;
class ULyraInventoryItemDefinition;

/**
 * @Hernan
 * ALyraWorldItemContainer
 * 
 * Actor that serves as world item container. It can be interacted to pick and drop (to be implemented) items.
 */
UCLASS()
class SHOOTERCORERUNTIME_API ALyraWorldItemContainer : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	// Constructor. This actor needs to be replicated.
	ALyraWorldItemContainer();

	// Interaction setup
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;

protected:
	// Overriden to: Sets this actor as a component receiver for Gameplay Feature Actions. An inventory manager component
	// will be added to this actor through Shooter Core Plugin
	virtual void BeginPlay() override;

	// Overriden to: Unset this actor as a component receiver for Gameplay Feature Actions
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Interaction option
	UPROPERTY(EditAnywhere)
	FInteractionOption Option;

	// Item definitions to be used as a template for the inventory manager component that will be added through the feature action
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ULyraInventoryItemDefinition>> ItemDefinitions;
};
