// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

#include "VrCharacter.h"

// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	Camera->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVrCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVrCharacter::MoveRight);
}

void AVrCharacter::MoveForward(float throttle)
{
	AddMovementInput(Camera->GetForwardVector(), throttle);
}

void AVrCharacter::MoveRight(float throttle)
{
	AddMovementInput(Camera->GetRightVector(), throttle);
}

