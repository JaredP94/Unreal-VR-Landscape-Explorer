// Fill out your copyright notice in the Description page of Project Settings.


#include "VrController.h"

#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AVrController::AVrController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Motion Controller"));
	SetRootComponent(MotionController);
}

void AVrController::SetHandOrientation(EControllerHand TargetHand)
{
	MotionController->SetTrackingSource(TargetHand);

	// Flip default mesh to match right controller
	if (TargetHand == EControllerHand::Right)
	{
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		this->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for (auto Component : StaticMeshComponents)
		{
			Component->SetWorldScale3D(FVector(1, -1, 1));
			Component->SetRelativeScale3D(FVector(1, -1, 1));
			Component->ReregisterComponent();
		}
	}
}

void AVrController::Grip()
{
	if (!bCanClimb) 
		return;

	if (!bIsClimbing)
	{
		bIsClimbing = true;
		ClimbInitiationLocation = GetActorLocation();

		PairedController->bIsClimbing = false;

		auto Character = Cast<ACharacter>(GetAttachParentActor());

		if (Character == nullptr)
			return;

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
}

void AVrController::Release()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;

		auto Character = Cast<ACharacter>(GetAttachParentActor());

		if (Character == nullptr)
			return;

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

// Called when the game starts or when spawned
void AVrController::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AVrController::HandleOnActorBeginOverlap);
}

// Called every frame
void AVrController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsClimbing)
	{
		auto HandControllerDelta = GetActorLocation() - ClimbInitiationLocation;
		GetAttachParentActor()->AddActorWorldOffset(-HandControllerDelta);
	}
}

void AVrController::HandleOnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	auto bClimbable = CanClimb();

	if (!bCanClimb && bClimbable)
	{
		auto ParentPawn = Cast<APawn>(this->GetAttachParentActor());

		if (ParentPawn != nullptr)
		{
			auto PlayerController = Cast<APlayerController>(ParentPawn->GetController());

			if (PlayerController != nullptr)
			{
				PlayerController->PlayHapticEffect(HapticFeedbackEffect, MotionController->GetTrackingSource());
			}
		}
	}

	bCanClimb = bClimbable;
}

void AVrController::HandleOnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
}

void AVrController::PairAdjacentController(AVrController* AdjacentController)
{
	PairedController = AdjacentController;
	PairedController->PairedController = this;
}

bool AVrController::CanClimb() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (auto OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->ActorHasTag(TEXT("Climbable")))
		{
			return true;
		}
	}

	return false;
}

