// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableItem.h"
#include "Net/UnrealNetwork.h"
#include "../Player/SurvivalCharacter.h"
#include "../Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "EquippableItem"

UEquippableItem::UEquippableItem()
{
    bStackable = false;
    bEquipped = false;
    UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}

void UEquippableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEquippableItem, bEquipped);
}

void UEquippableItem::Use(ASurvivalCharacter* Character)
{
    if (Character && Character->HasAuthority())
    {
        if (Character->GetEquippedItems().Contains(Slot) && !bEquipped)
        {
            UEquippableItem* AllreadyEquippedItem = *Character->GetEquippedItems().Find(Slot);
            AllreadyEquippedItem->SetEquipped(true);
        }

        SetEquipped(!IsEquipped());
    }

}

bool UEquippableItem::Equip(ASurvivalCharacter* Character)
{
    if (Character)
    {
        return Character->EquipItem(this);
    }
    return false;
}

bool UEquippableItem::UnEquip(ASurvivalCharacter* Character)
{
    if (Character)
    {
        return Character->UnEquipItem(this);
    }
    return false;
}

bool UEquippableItem::ShouldShowInInventory() const
{
    return !bEquipped;
}

void UEquippableItem::SetEquipped(bool bNewEquipped)
{
    bEquipped = bNewEquipped;
    EquipStatusChanged();
    MarkDirtyForReplication();
}

void UEquippableItem::EquipStatusChanged()
{
    if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOuter()))
    {
        if (bEquipped)
        {
            Equip(Character);
        }
        else
        {
            UnEquip(Character);
        }

    }

    // Tell ui to update
    OnItemModified.Broadcast();

}


#undef LOCTEXT_NAMESPACE