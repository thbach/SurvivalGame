// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Components/ActorComponent.h"
#include "../Items/Item.h"

#define LOCTEXT_NAMESPACE "Inventory"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicated(true);
}

FItemAddResult UInventoryComponent::TryAddItem(UItem* Item)
{
	return TryAddItem_Internal(Item);
}

FItemAddResult UInventoryComponent::TryAddItemFromClass(TSubclassOf<UItem> ItemClass, const int32 Quantity)
{
	UItem* Item = NewObject<UItem>(GetOwner(), ItemClass);
	Item->SetQuantity(Quantity);
	return TryAddItem_Internal(Item);
}

int32 UInventoryComponent::ConsumeItem(UItem* Item)
{
	if (Item)
	{
		ConsumeItem(Item, Item->GetQuantity());
	}
	return 0;
}

int32 UInventoryComponent::ConsumeItem(UItem* Item, const int32 Quantity)
{
	if (IsServer() && Item)
	{
		const int32 RemoveQuantity = FMath::Min(Quantity, Item->GetQuantity());
		ensure(!(Item->GetQuantity() - RemoveQuantity < 0)); // no negative quantiy

		Item->SetQuantity(Item->GetQuantity() - RemoveQuantity); // We now have zero of this item, remove it from the inventory

		if (Item->GetQuantity() <= 0)
		{
			RemoveItem(Item);
		}
		else
		{
			ClientRefreshInventory();
		}
	}
	return 0;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if (IsServer())
	{
		if (Item)
		{
			Items.RemoveSingle(Item);
			ReplicatedItemsKey++;
			return true;
		}

	}
	return false;
}

bool UInventoryComponent::HasItem(TSubclassOf<UItem> ItemClass, const int32 Quantity /*= 1*/) const
{
	if (UItem* ItemsToFind = FindItemByClass(ItemClass))
	{
		return ItemsToFind->GetQuantity() >= Quantity;
	}
	return false;
}

UItem* UInventoryComponent::FindItem(UItem* Item) const
{
	if (Item)
	{
		for (auto& InvItem : Items)
		{
			if (InvItem && InvItem->GetClass() == Item->GetClass())
			{
				return InvItem;
			}

		}

	}
	return nullptr;
}

UItem* UInventoryComponent::FindItemByClass(TSubclassOf<UItem> ItemClass) const
{
	for (auto& InvItem : Items)
	{
		if (InvItem && InvItem->GetClass() == ItemClass)
		{
			return InvItem;
		}
	}
	return nullptr;
}

TArray<UItem*> UInventoryComponent::FindItemsByClass(TSubclassOf<UItem> ItemClass) const
{
	TArray<UItem*> ItemsOfClass;

	for (auto& InvItem : Items)
	{
		if (InvItem && InvItem->GetClass() == ItemClass)
		{
			ItemsOfClass.Add(InvItem);
		}
	}
	return ItemsOfClass;
}

float UInventoryComponent::GetCurrentWeight() const
{
	float Weight = 0.f;
	for (auto& Item : Items)
	{
		if (Item)
		{
			Weight += Item->GetStackWeight();
		}
	}
	return Weight;
}

void UInventoryComponent::SetWeightCapacity(const float NewWeightCapacity)
{
	WeightCapacity = NewWeightCapacity;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	Capacity = NewCapacity;
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::ClientRefreshInventory_Implementation()
{
	OnInventoryUpdated.Broadcast();
}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);

}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel *Channel, FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Check if the array of items needs to replicate
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		for (auto& Item : Items)
		{
			if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->RepKey))
			{
				bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}
	}

	return bWroteSomething;
}

UItem* UInventoryComponent::AddItem(UItem* Item)
{
	if(IsServer())
	{
		UItem* NewItem = NewObject<UItem>(GetOwner(), Item->GetClass());
		NewItem->SetQuantity(Item->GetQuantity());
		NewItem->OwningInventory = this;
		NewItem->AddedToInventory(this);
		Items.Add(NewItem);
		NewItem->MarkDirtyForReplication();

		return NewItem;
	}

	return nullptr;
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
}

FItemAddResult UInventoryComponent::TryAddItem_Internal(UItem* Item)
{
	if (IsServer())
	{
		const int32 AddAmount = Item->GetQuantity();

		// Check if room in inventory
		if (Items.Num() + 1 > GetCapacity())
		{
			return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryCapacityFullText", "Couldn't add item to the Inventory. Inventory is full."));
		}

		// check if weight is too much
		if (!FMath::IsNearlyZero(Item->Weight))
		{
			if (GetCurrentWeight() + Item->Weight > GetWeightCapacity())
			{
				return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryTooMuchWeightText", "Couldn't add item to inventory. Carrying too much weight."));
			}
		}

		// if the item is stackable, check if we already have it and add it to their stgack
		if (Item->bStackable)
		{
			// Somehow the items quantity went over the max stack size. This shouldn't ever happen
			ensure(Item->GetQuantity() <= Item->MaxStackSize);

			if (UItem* ExistingItem = FindItem(Item))
			{
				if (ExistingItem->GetQuantity() < ExistingItem->MaxStackSize)
				{
					// Find out how much of the item to add
					const int32 CapacityMaxAddAmount = ExistingItem->MaxStackSize - ExistingItem->GetQuantity();
					int32 ActualAddAmount = FMath::Min(AddAmount, CapacityMaxAddAmount);

					FText ErrorText = LOCTEXT("InventoryErrorText", "Couldn't add all of the item to your inventory.");

					// Adjust based on how much weight we can carry
					if (!FMath::IsNearlyZero(Item->Weight))
					{
						// Find the maximum amount of the item we could take due to weight
						const int32 WeightMaxAddAmount = FMath::FloorToInt((WeightCapacity - GetCurrentWeight()) / Item->Weight);
						ActualAddAmount = FMath::Min(ActualAddAmount, WeightMaxAddAmount);

						if (ActualAddAmount < AddAmount)
						{
							ErrorText = FText::Format(LOCTEXT("InventoryTooMuchWeightText", "Couldn't add entire stack of {ItemName} to Inventory."), Item->ItemDisplayName);
						}
					}
					else if (ActualAddAmount < AddAmount)
					{
						// If the item weights none and wer cant take it, then there was a capcity issue
						ErrorText = FText::Format(LOCTEXT("InventoryCapacityFullText", "Couldn't add entire stack of {ItemName} to Inventory. Inventory was full."), Item->ItemDisplayName);
					}

					// We couldnt add any of the item to our inventory
					if (ActualAddAmount <= 0)
					{
						return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryErrorText", "Couldn't add item to inventory/"));
					}

					ExistingItem->SetQuantity(ExistingItem->GetQuantity() + ActualAddAmount);

					ensure(ExistingItem->GetQuantity() <= ExistingItem->MaxStackSize);

					if (ActualAddAmount < AddAmount)
					{
						return FItemAddResult::AddedSome(AddAmount, ActualAddAmount, ErrorText);
					}
					else
					{
						return FItemAddResult::AddedAll(AddAmount);
					}
				}
				else
				{
					return FItemAddResult::AddedNone(AddAmount, FText::Format(LOCTEXT("InventoryFullStackText", "Couldn't add {ItemName}. You already have a full stack of this Item"), Item->ItemDisplayName));
				}
			}
			else
			{
				// Since we dont have any of this item, we'll add the full stack
				AddItem(Item);
				return FItemAddResult::AddedAll(AddAmount);
			}
		}
		else // Item is not stackable
		{
			// Non-stackables should always have a quantity of 1
			ensure(Item->GetQuantity() == 1);

			AddItem(Item);
			return FItemAddResult::AddedAll(AddAmount);
		}
	}

	// AddItem should never be called on a client
	check(false);
	return FItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

bool UInventoryComponent::IsServer()
{
	return GetOwner() && GetOwner()->HasAuthority();
}

#undef LOCTEXT_NAMESPACE