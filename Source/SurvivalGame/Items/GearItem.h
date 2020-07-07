// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippableItem.h"
#include "GearItem.generated.h"

class USkeletalMesh;
class UMaterialInstance;
class ASurvivalCharacter;

UCLASS(Blueprintable)
class SURVIVALGAME_API UGearItem : public UEquippableItem
{
	GENERATED_BODY()

public:

	UGearItem();

	virtual bool Equip(ASurvivalCharacter* Character) override;
	virtual bool UnEquip(ASurvivalCharacter* Character) override;

	// The skeletal mesh for this gear
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	USkeletalMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear")
	UMaterialInstance* MaterialInstance;

	// the amout of defence this item provbeds 0.2 = 20% less damage taken
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gear", meta = (ClampMin = 0.0, ClampMax = 1.0))
	float DamageDefenceMultiplier;


};
