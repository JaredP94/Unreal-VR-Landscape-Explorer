// Fill out your copyright notice in the Description page of Project Settings.

#include "VrCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	CameraHolder = CreateDefaultSubobject<USceneComponent>(TEXT("VR Camera Holder"));
	CameraHolder->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(CameraHolder);
	CameraHolder->RelativeLocation.Set(0.f, 0.f, -90.15f);

	DestinationIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Destination Indicator"));
	DestinationIndicator->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDestinationIndicator();
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

void AVrCharacter::UpdateDestinationIndicator()
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * MaxTeleporationDistance);

	auto bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (bHit)
	{
		DestinationIndicator->SetWorldLocation(HitResult.Location);
	}

	DestinationIndicator->SetVisibility(bHit);
}

