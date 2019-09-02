// Fill out your copyright notice in the Description page of Project Settings.


#include "VrController.h"

#include "MotionControllerComponent.h"

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
}

// Called when the game starts or when spawned
void AVrController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVrController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

