// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetMesh(), "CameraSocket");
	CameraComponent->bUsePawnControlRotation = true;

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	// HelmetMesh->SetMasterPoseComponent(GetMesh(), true);

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(GetMesh());
	// ChestMesh->SetMasterPoseComponent(GetMesh(), true);

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	// LegsMesh->SetMasterPoseComponent(GetMesh());

	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetupAttachment(GetMesh());
	// FeetMesh->SetMasterPoseComponent(GetMesh());

	VestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VestMesh"));
	VestMesh->SetupAttachment(GetMesh());
	// VestMesh->SetMasterPoseComponent(GetMesh());

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());
	// HandsMesh->SetMasterPoseComponent(GetMesh());

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh"));
	BackpackMesh->SetupAttachment(GetMesh());
	// BackpackMesh->SetMasterPoseComponent(GetMesh());

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetMesh()->SetOwnerNoSee(true);

}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASurvivalCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASurvivalCharacter::Turn);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASurvivalCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASurvivalCharacter::StopCrouching);

}

void ASurvivalCharacter::MoveForward(float AxisValue)
{
	if (AxisValue == 0.0f) return;
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void ASurvivalCharacter::MoveRight(float AxisValue)
{
	if (AxisValue == 0.0f) return;
	AddMovementInput(GetActorRightVector(), AxisValue);
}

void ASurvivalCharacter::LookUp(float AxisValue)
{
	if (AxisValue == 0.0f) return;
	AddControllerPitchInput(AxisValue);
}

void ASurvivalCharacter::Turn(float AxisValue)
{
	if (AxisValue == 0.0f) return;
	AddControllerYawInput(AxisValue);
}

void ASurvivalCharacter::StartCrouching()
{
	Crouch();
}

void ASurvivalCharacter::StopCrouching()
{
	UnCrouch();
}

