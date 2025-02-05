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

UCLASS()
class SHOOTERCORERUNTIME_API ALyraWorldItemContainer : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	ALyraWorldItemContainer();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;

protected:
	UPROPERTY(EditAnywhere)
	FInteractionOption Option;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ULyraInventoryItemDefinition>> ItemDefinitions;
};
