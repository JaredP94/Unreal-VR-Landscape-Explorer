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
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

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

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process Component"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	VrPlayerController = Cast<APlayerController>(GetController());

	if (BaseBlinkerMaterial == nullptr)
		return;

	InstanceBlinkerMaterial = UMaterialInstanceDynamic::Create(BaseBlinkerMaterial, this);
	PostProcessComponent->AddOrUpdateBlendable(InstanceBlinkerMaterial);
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDestinationIndicator();
	UpdateBlinkers();
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
	if (VrPlayerController == nullptr)
		return;

	VrPlayerController->PlayerCameraManager->StartCameraFade(0.f, 1.f, TeleportationFadeDuration, FLinearColor::Black, false, true);

	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &AVrCharacter::FinaliseTeleport, TeleportationFadeDuration);
}

void AVrCharacter::FinaliseTeleport()
{
	if (VrPlayerController == nullptr)
		return;

	auto TargetLocation = DestinationIndicator->GetComponentLocation();
	SetActorLocation(FVector(TargetLocation.X, TargetLocation.Y, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 90.15f));
	VrPlayerController->PlayerCameraManager->StartCameraFade(1.f, 0.f, TeleportationFadeDuration, FLinearColor::Black, false, false);
}

void AVrCharacter::UpdateDestinationIndicator()
{
	FVector TargetLocation;

	auto bValidLocation = LocateTeleportDestination(TargetLocation);

	if (bValidLocation)
	{
		DestinationIndicator->SetWorldLocation(TargetLocation);
	}

	DestinationIndicator->SetVisibility(bValidLocation);
}

bool AVrCharacter::LocateTeleportDestination(FVector& OutLocation)
{
	FHitResult HitResult;
	auto Start = Camera->GetComponentLocation();
	auto End = Start + (Camera->GetForwardVector() * MaxTeleporationDistance);
	auto bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (!bHit)
		return false;

	FNavLocation NavLocation;
	auto navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	bool bOnNavMesh = navSystem->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportationProjectionExtent);

	if (!bOnNavMesh)
		return false;

	OutLocation = NavLocation.Location;

	return true;
}

void AVrCharacter::UpdateBlinkers()
{
	if (CurveRadiusVsVelocity == nullptr)
		return;

	auto Speed = GetVelocity().Size();
	auto Radius = CurveRadiusVsVelocity->GetFloatValue(Speed);
	InstanceBlinkerMaterial->SetScalarParameterValue(TEXT("Radius"), Radius);

	auto BlinkerCenterPosition = GetBlinkerCenterPosition();
	InstanceBlinkerMaterial->SetVectorParameterValue(TEXT("CenterPosition"), FLinearColor(BlinkerCenterPosition.X, BlinkerCenterPosition.Y, 0.f));
}

FVector2D AVrCharacter::GetBlinkerCenterPosition()
{
	auto MovementDirection = GetVelocity().GetSafeNormal();

	if (MovementDirection.IsNearlyZero() || VrPlayerController == nullptr)
	{
		return FVector2D(0.5f, 0.5f);
	}

	FVector WorldLocation;

	if (FVector::DotProduct(Camera->GetForwardVector(), MovementDirection) > 0.f)
	{
		WorldLocation = Camera->GetComponentLocation() + (MovementDirection * 1000);
	}
	else
	{
		WorldLocation = Camera->GetComponentLocation() - (MovementDirection * 1000);
	}
	

	FVector2D ScreenLocation;
	VrPlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);

	int ViewportX, ViewportY;
	VrPlayerController->GetViewportSize(ViewportX, ViewportY);
	ScreenLocation.X /= ViewportX;
	ScreenLocation.Y /= ViewportY;

	return ScreenLocation;
}

