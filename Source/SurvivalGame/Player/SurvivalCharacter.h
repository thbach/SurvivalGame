// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

class UCameraComponent;
class UItem;
class APickup;
class USkeletalMeshComponent;
class UInteractionComponent;
class UInventoryComponent;
class UEquippableItem;
class UGearItem;

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	UPROPERTY()
	UInteractionComponent* ViewedInteractionComponent; // The current interactable component we're viewing

	UPROPERTY()
	float LastInteractionCheckTime; // The time when we last checked for an interactable

	UPROPERTY()
	bool bInteractHeld; // whether the local player is holding the interact key
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemsChanged, const EEquippableSlot, Slot, const UEquippableItem*, Item);

UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivalCharacter();

	// The mesh to have equipped if we dont have an item equipped
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
	TMap<EEquippableSlot, USkeletalMesh*> NakedMeshes;

	// The players body meshes
	UPROPERTY(BlueprintReadOnly, Category = Mesh)
	TMap<EEquippableSlot, USkeletalMeshComponent* > PlayerMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* PlayerInventory;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* HelmetMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* ChestMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* LegsMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* FeetMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* VestMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* HandsMesh;
	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
	USkeletalMeshComponent* BackpackMesh;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void Restart() override;

// LOOTING
public:
	UFUNCTION(BlueprintCallable)
	void SetLootSource(UInventoryComponent* NewLootSource);

	UFUNCTION(BlueprintPure, Category = "Looting")
	bool IsLooting() const;

protected:

	// Begin being looted by a player
	UFUNCTION()
	void BeginLootingPlayer(ASurvivalCharacter* Character);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetLootSource(UInventoryComponent* NewLootSource);

	// The inventory that we are currently looking from
	UPROPERTY(ReplicatedUsing = OnRep_LootSource, BlueprintReadOnly)
	UInventoryComponent* LootSource;

	UFUNCTION()
	void OnLootSourceOwnerDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void OnRep_LootSource();

public:

	UFUNCTION(BlueprintCallable, Category = "Looting")
	void LootItem(UItem* ItemToGive);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLootItem(UItem* ItemToLoot);


	// Interaction
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance; // for RayCasting

	void PerformInteractionCheck();

	void CouldntFindInteractable();
	void FoundNewInteractable(UInteractionComponent* Interactable);

	void BeginInteract();
	void EndInteract();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginInteract();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	UPROPERTY()
	FInteractionData InteractionData; // Info about the current state of the players interaction

	UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }

	FTimerHandle TimerHandle_Interact;

public:
	// True if we're interacting with an item that has an interaction time (eg a lamp that takes 2 seconds to turn on)
	bool IsInteracting() const;

	// Get the time till we interact with the current interactable
	float GetRemainingInteractTime() const;

	// Items

	// [Server] Use an item form our inventory
	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(UItem* Item);

	// [Server] Drop an item
	UFUNCTION(BlueprintCallable, Category = "Items")
	void DropItem(UItem* Item, const int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropItem(UItem* Item, const int32 Quantity);

	// We need this becasue the pickups use a blueprint base class
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<APickup> PickupClass;

public:

	bool EquipItem(UEquippableItem* Item);
	bool UnEquipItem(UEquippableItem* Item);

	void EquipGear(UGearItem* Gear);
	void UnEquipGear(const EEquippableSlot Slot);

	UPROPERTY(BlueprintAssignable, Category = "Items")
	FOnEquippedItemsChanged OnEquippedItemsChanged;

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetSlotSkeletalMeshComponent(const EEquippableSlot Slot);

	UFUNCTION(BlueprintPure)
	TMap<EEquippableSlot, UEquippableItem*> GetEquippedItems() const { return EquippedItems; }

protected:
	// Allows for efficient access of equipped items
	UPROPERTY(VisibleAnywhere, Category = "Items")
	TMap<EEquippableSlot, UEquippableItem* > EquippedItems;


	// Movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void Turn(float AxisValue);
	void StartCrouching();
	void StopCrouching();

public:


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
