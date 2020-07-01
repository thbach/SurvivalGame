// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;

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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void Turn(float AxisValue);
	void StartCrouching();
	void StopCrouching();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
