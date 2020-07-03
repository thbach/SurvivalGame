// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UInteractionComponent;

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

UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivalCharacter();

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
	virtual void Tick(float DeltaTime) override;

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


protected:
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
