// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "FoodItem.generated.h"

class ASurvivalCharacter;

UCLASS()
class SURVIVALGAME_API UFoodItem : public UItem
{
	GENERATED_BODY()

public:

	UFoodItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Healing")
	float HealAmount;

	virtual void Use(ASurvivalCharacter* Character) override;

};
