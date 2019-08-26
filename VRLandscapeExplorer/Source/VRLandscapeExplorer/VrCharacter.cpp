// Fill out your copyright notice in the Description page of Project Settings.

#include "VrCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"

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
	PlayerInputComponent->BindAction(TEXT("Teleport"), EInputEvent::IE_Released, this, &AVrCharacter::InitiateTeleport);
}

void AVrCharacter::MoveForward(float throttle)
{
	AddMovementInput(Camera->GetForwardVector(), throttle);
}

void AVrCharacter::MoveRight(float throttle)
{
	AddMovementInput(Camera->GetRightVector(), throttle);
}

void AVrCharacter::InitiateTeleport()
{
	auto PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController == nullptr)
		return;

	PlayerController->PlayerCameraManager->StartCameraFade(0.f, 1.f, TeleportationFadeDuration, FLinearColor::Black, false, true);

	FTimerHandle timerHandle;

	GetWorldTimerManager().SetTimer(timerHandle, this, &AVrCharacter::FinaliseTeleport, TeleportationFadeDuration);
}

void AVrCharacter::FinaliseTeleport()
{
	auto PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController == nullptr)
		return;

	auto TargetLocation = DestinationIndicator->GetComponentLocation();
	SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, GetActorLocation().Z));
	PlayerController->PlayerCameraManager->StartCameraFade(1.f, 0.f, TeleportationFadeDuration, FLinearColor::Black, false, false);
}

void AVrCharacter::UpdateDestinationIndicator()
{
	FHitResult HitResult;
	auto Start = Camera->GetComponentLocation();
	auto End = Start + (Camera->GetForwardVector() * MaxTeleporationDistance);

	auto bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	FNavLocation NavLocation;
	auto navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	bool bOnNavMesh = navSystem->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportationProjectionExtent);

	if (bHit && bOnNavMesh)
	{
		DestinationIndicator->SetWorldLocation(NavLocation.Location);
	}

	DestinationIndicator->SetVisibility(bHit && bOnNavMesh);
}

