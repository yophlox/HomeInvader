// Copyright YoPhlox. All Rights Reserved.

#include "FPSController.h"

AFPSController::AFPSController()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	cam->SetRelativeLocation(FVector(0.0, 0.0f, 40.0f));

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	DefaultCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	GetCharacterMovement()->JumpZVelocity = JumpForce;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GravityScale = 1.2f; 

}

void AFPSController::BeginPlay()
{
	Super::BeginPlay();
	InitialCameraLocation = cam->GetRelativeLocation();
}

void AFPSController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity = GetVelocity();
	if (Velocity.Size() > 0)
	{
		HeadBobTimer += DeltaTime * HeadBobSpeed;
		float BobOffset = FMath::Sin(HeadBobTimer) * HeadBobAmount;
		cam->SetRelativeLocation(FVector(InitialCameraLocation.X, InitialCameraLocation.Y, 
			InitialCameraLocation.Z + BobOffset));
	}
	else
	{
		HeadBobTimer = 0.0f;
		cam->SetRelativeLocation(InitialCameraLocation);
	}

	float CameraPitch = cam->GetRelativeRotation().Pitch;
	float SpeedMultiplier = 1.0f - FMath::Clamp(FMath::Abs(CameraPitch) / 90.0f, 0.0f, 1.0f);
	
	float BaseSpeed;
	if (bIsCrouching)
	{
		BaseSpeed = CrouchSpeed;
	}
	else if (bIsSprinting)
	{
		BaseSpeed = SprintSpeed;
	}
	else
	{
		BaseSpeed = DefaultWalkSpeed;
	}

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;

	if (GetCharacterMovement()->IsFalling())
	{
		FVector CurrentVelocity = GetVelocity();
		if (CurrentVelocity.Z < 0)  
		{
			if (GetInputAxisValue("Vert") > 0)
			{
				AddMovementInput(GetActorForwardVector(), AirControlForce * DeltaTime);
			}
		}
	}
}

void AFPSController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Hori", this, &AFPSController::HoriMove);
	InputComponent->BindAxis("Vert", this, &AFPSController::VertMove);

	InputComponent->BindAxis("HoriRot", this, &AFPSController::HoriRot);
	InputComponent->BindAxis("VertRot", this, &AFPSController::VertRot);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSController::StartCrouch);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AFPSController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AFPSController::StopSprint);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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
		if(temp <= 90.0f && temp >= -90.0f)
		{
			cam->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
		}
	}
}

void AFPSController::StartCrouch()
{
	if (!bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHeight * 0.5f);
		cam->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
		bIsCrouching = true;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHeight);
		cam->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
		bIsCrouching = false;
	}
}

void AFPSController::StartSprint()
{
	if (!bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsSprinting = true;
	}
}

void AFPSController::StopSprint()
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		bIsSprinting = false;
	}
}
