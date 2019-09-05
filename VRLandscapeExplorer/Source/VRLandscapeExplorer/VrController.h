// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VrController.generated.h"

UCLASS()
class VRLANDSCAPEEXPLORER_API AVrController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVrController();

	void SetHandOrientation(EControllerHand TargetHand);
	void Grip();
	void Release();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "VR Controller")
	class UMotionControllerComponent* MotionController;

	UPROPERTY(EditDefaultsOnly, Category = "Haptic Feedback")
	class UHapticFeedbackEffect_Base* HapticFeedbackEffect;

private:
	UFUNCTION()
	void HandleOnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void HandleOnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool CanClimb() const;

	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbInitiationLocation;
};
