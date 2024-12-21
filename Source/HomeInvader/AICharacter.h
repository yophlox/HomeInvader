// Copyright YoPhlox. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "AICharacter.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrolling,
	Investigating,
	Chasing,
	Searching,
	Returning
};

UCLASS()
class HOMEINVADER_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAICharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ChaseSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HearingRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightAngle = 90.0f;

private:
	UFUNCTION()
	void OnPlayerSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	void MoveToNextPatrolPoint();
	void ChasePlayer();
	void InvestigateSound(const FVector& SoundLocation);
	void UpdateAIState();
	void SearchForPlayer();
	void ReturnToPatrol();

	EAIState CurrentState;
	class AAIController* AIController;
	int32 CurrentPatrolPoint;
	FVector LastKnownPlayerLocation;
	FTimerHandle SearchTimerHandle;
	float SearchDuration = 30.0f;
	
	UPROPERTY()
	class AFPSController* PlayerRef;
};
