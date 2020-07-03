// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItem;
class UActorChannel;
class FOutBunch;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetWeightCapacity() const { return WeightCapacity; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UItem*> GetItems() const { return Items; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, Category = "Inventory")
	TArray<UItem*> Items;

	// The maximum weight the inventory can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float WeightCapacity;

	// The maximum number of items the inventory can hold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0, ClampMax = 200))
	int32 Capacity;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlags) override;

private:

	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;


};
