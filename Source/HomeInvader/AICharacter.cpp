// Copyright YoPhlox. All Rights Reserved.

#include "AICharacter.h"
#include "FPSController.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAICharacter::AAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(SightAngle);
	PawnSensingComp->SightRadius = SightRange;
	PawnSensingComp->HearingThreshold = HearingRange;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());
	CurrentState = EAIState::Patrolling;
	
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAICharacter::OnPlayerSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AAICharacter::OnNoiseHeard);
	
	PlayerRef = Cast<AFPSController>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if (PatrolPoints.Num() > 0)
	{
		MoveToNextPatrolPoint();
	}
}

void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAIState();
}

void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAICharacter::OnPlayerSeen(APawn* SeenPawn)
{
	if (Cast<AFPSController>(SeenPawn))
	{
		LastKnownPlayerLocation = SeenPawn->GetActorLocation();
		CurrentState = EAIState::Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

void AAICharacter::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (CurrentState != EAIState::Chasing && Cast<AFPSController>(NoiseInstigator))
	{
		CurrentState = EAIState::Investigating;
		InvestigateSound(Location);
	}
}

void AAICharacter::MoveToNextPatrolPoint()
{
	if (PatrolPoints.Num() == 0) return;
	
	CurrentPatrolPoint = (CurrentPatrolPoint + 1) % PatrolPoints.Num();
	if (AIController)
	{
		AIController->MoveToActor(PatrolPoints[CurrentPatrolPoint]);
	}
}

void AAICharacter::ChasePlayer()
{
	if (PlayerRef && AIController)
	{
		AIController->MoveToActor(PlayerRef);
		LastKnownPlayerLocation = PlayerRef->GetActorLocation();
	}
}

void AAICharacter::InvestigateSound(const FVector& SoundLocation)
{
	if (AIController)
	{
		AIController->MoveToLocation(SoundLocation);
	}
}

void AAICharacter::SearchForPlayer()
{
	if (AIController)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (NavSystem)
		{
			FNavLocation RandomPoint;
			if (NavSystem->GetRandomPointInNavigableRadius(LastKnownPlayerLocation, 500.0f, RandomPoint))
			{
				AIController->MoveToLocation(RandomPoint.Location);
			}
		}
	}
}

void AAICharacter::ReturnToPatrol()
{
	CurrentState = EAIState::Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	MoveToNextPatrolPoint();
}

void AAICharacter::UpdateAIState()
{
	switch (CurrentState)
	{
		case EAIState::Patrolling:
			if (AIController && AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				MoveToNextPatrolPoint();
			}
			break;
			
		case EAIState::Chasing:
			if (PlayerRef)
			{
				float DistanceToPlayer = FVector::Distance(GetActorLocation(), PlayerRef->GetActorLocation());
				if (DistanceToPlayer > SightRange * 1.5f)
				{
					CurrentState = EAIState::Searching;
					GetWorld()->GetTimerManager().SetTimer(SearchTimerHandle, 
						[this]() { ReturnToPatrol(); }, 
						SearchDuration, false);
				}
				else
				{
					ChasePlayer();
				}
			}
			break;
			
		case EAIState::Investigating:
			if (AIController && AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				ReturnToPatrol();
			}
			break;
			
		case EAIState::Searching:
			if (AIController && AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				SearchForPlayer();
			}
			break;
	}
}

