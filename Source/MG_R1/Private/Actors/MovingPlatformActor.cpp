#include "Actors/MovingPlatformActor.h"
#include "Components/StaticMeshComponent.h"

AMovingPlatformActor::AMovingPlatformActor()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);
	PlatformMesh->SetMobility(EComponentMobility::Movable);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

void AMovingPlatformActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentTarget = PointB ? PointB.Get() : PointA.Get();
	CurrentSpeed = MinSpeed;
}

AActor* AMovingPlatformActor::GetOtherPoint(const AActor* Point) const
{
	if (Point == PointA)
	{
		return PointB;
	}
	if (Point == PointB)
	{
		return PointA;
	}
	return PointB ? PointB.Get() : PointA.Get();
}

FVector AMovingPlatformActor::GetPathTargetLocation() const
{
	return CurrentTarget ? CurrentTarget->GetActorLocation() : GetActorLocation();
}

void AMovingPlatformActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PointA || !PointB || !CurrentTarget)
	{
		return;
	}

	const FVector CurrentLocation = GetActorLocation();
	const FVector TargetLocation = GetPathTargetLocation();
	const FVector ToTarget = TargetLocation - CurrentLocation;
	const float Distance = ToTarget.Size();

	if (Distance <= TurnDistance)
	{
		CurrentTarget = GetOtherPoint(CurrentTarget);
		CurrentSpeed = MinSpeed;
		return;
	}

	const float BrakeDistance = (SlowDownDistance > 0.f) ? SlowDownDistance : (TurnDistance * 4.f);
	const float DesiredSpeed = (Distance < BrakeDistance)
		? FMath::GetMappedRangeValueClamped(FVector2D(TurnDistance, BrakeDistance), FVector2D(MinSpeed, MaxSpeed), Distance)
		: MaxSpeed;

	if (CurrentSpeed < DesiredSpeed)
	{
		CurrentSpeed = FMath::Min(CurrentSpeed + Acceleration * DeltaTime, DesiredSpeed);
	}
	else
	{
		CurrentSpeed = FMath::Max(CurrentSpeed - Acceleration * DeltaTime, DesiredSpeed);
	}

	const FVector Direction = ToTarget / Distance;
	const FVector NewLocation = CurrentLocation + Direction * CurrentSpeed * DeltaTime;
	SetActorLocation(NewLocation, true);
}
