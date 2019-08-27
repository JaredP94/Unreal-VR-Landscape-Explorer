// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VrCharacter.generated.h"

UCLASS()
class VRLANDSCAPEEXPLORER_API AVrCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVrCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USceneComponent* CameraHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UStaticMeshComponent* DestinationIndicator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Post Process")
	class UPostProcessComponent* PostProcessComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Post Process")
	class UMaterialInstanceDynamic* InstanceBlinkerMaterial;

private:
	void MoveForward(float throttle);
	void MoveRight(float throttle);
	void InitiateTeleport();
	void FinaliseTeleport();
	void UpdateDestinationIndicator();
	bool LocateTeleportDestination(FVector &OutLocation);
	void UpdateBlinkers();

	APlayerController* VrPlayerController;

private:
	UPROPERTY(EditAnywhere)
	float MaxTeleporationDistance = 1000;

	UPROPERTY(EditAnywhere)
	float TeleportationFadeDuration = 0.5f;

	UPROPERTY(EditAnywhere)
	FVector TeleportationProjectionExtent = FVector(100.f, 100.f, 100.f);

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BaseBlinkerMaterial;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* CurveRadiusVsVelocity;
};
