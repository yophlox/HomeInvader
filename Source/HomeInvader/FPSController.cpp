// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSController.h"

// Sets default values
AFPSController::AFPSController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->AttachTo(RootComponent);
	cam->SetRelativeLocation(FVector(0.0, 0.0f, 40.0f));

}

// Called when the game starts or when spawned
void AFPSController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Hori", this, &AFPSController::HoriMove);
	InputComponent->BindAxis("Vert", this, &AFPSController::VertMove);

	InputComponent->BindAxis("HoriRot", this, &AFPSController::HoriRot);
	InputComponent->BindAxis("VertRot", this, &AFPSController::VertRot);

}

void AFPSController::HoriMove(float value)
{
	if (value)
	{
		AddMovementInput(GetActorRightVector(), value);
	}
}

void AFPSController::VertMove(float value)
{
	if (value)
	{
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void AFPSController::HoriRot(float value)
{
	if (value)
	{
		AddActorLocalRotation(FRotator(0.0f, value, 0.0f));
	}
}

void AFPSController::VertRot(float value)
{
	if (value)
	{
		float temp = cam->GetRelativeRotation().Pitch + value;
		if(temp < 65.0f && temp > -65.0f)
		{
			cam->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
		}
	}
}
