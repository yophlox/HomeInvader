// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "FPSController.generated.h"

UCLASS()
class HOMEINVADER_API AFPSController : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void HoriMove(float value);
	void VertMove(float value);

	void HoriRot(float value);
	void VertRot(float value);

	void StartCrouch();
	void StopCrouch();

	void StartSprint();
	void StopSprint();

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* cam;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float DefaultWalkSpeed = 600.0f;
		float CrouchSpeed = 125.0f;
		float DefaultCapsuleHeight;
		bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintSpeed = 1000.0f;
		bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float HeadBobSpeed = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "Camera")
		float HeadBobAmount = 5.0f;
	
	 float HeadBobTimer = 0.0f;
		FVector InitialCameraLocation;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float JumpForce = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float AirControlForce = 100.0f;
};
