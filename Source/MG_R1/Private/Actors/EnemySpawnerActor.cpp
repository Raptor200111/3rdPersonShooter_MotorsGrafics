#include "Actors/EnemySpawnerActor.h"
#include "Actors/EnemyCubeActor.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawnerActor::AEnemySpawnerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaPreview = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaPreview"));
	SetRootComponent(AreaPreview);
	AreaPreview->SetBoxExtent(FVector(HalfExtent, HalfExtent, 10.f));
	AreaPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaPreview->SetHiddenInGame(true);
	AreaPreview->ShapeColor = FColor::Orange;
	AreaPreview->bDrawOnlyIfSelected = true;

	EnemyClass = AEnemyCubeActor::StaticClass();
}

void AEnemySpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	AreaPreview->SetBoxExtent(FVector(HalfExtent, HalfExtent, 10.f));
	EnsureEnemyCount();
}

void AEnemySpawnerActor::EnsureEnemyCount()
{
	AliveEnemies.RemoveAll([](const TObjectPtr<AEnemyCubeActor>& E)
	{
		return !IsValid(E) || !E->IsAlive();
	});

	while (AliveEnemies.Num() < MaxAlive)
	{
		SpawnOneEnemy();
	}
}

void AEnemySpawnerActor::SpawnOneEnemy()
{
	UWorld* World = GetWorld();
	if (!World || !EnemyClass)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;

	AEnemyCubeActor* Enemy = World->SpawnActor<AEnemyCubeActor>(
		EnemyClass,
		PickSpawnLocation(),
		FRotator::ZeroRotator,
		Params);

	if (!Enemy)
	{
		return;
	}

	Enemy->OnEnemyDied.AddDynamic(this, &AEnemySpawnerActor::HandleEnemyDied);
	AliveEnemies.Add(Enemy);
}

FVector AEnemySpawnerActor::PickSpawnLocation() const
{
	const FVector Origin = GetActorLocation();
	for (int32 Attempt = 0; Attempt < 12; ++Attempt)
	{
		const float X = FMath::FRandRange(-HalfExtent, HalfExtent);
		const float Y = FMath::FRandRange(-HalfExtent, HalfExtent);
		const FVector Candidate(Origin.X + X, Origin.Y + Y, Origin.Z + SpawnHeight);

		bool bTooClose = false;
		for (const TObjectPtr<AEnemyCubeActor>& E : AliveEnemies)
		{
			if (IsValid(E) && FVector::DistSquared(E->GetActorLocation(), Candidate) < (150.f * 150.f))
			{
				bTooClose = true;
				break;
			}
		}
		if (!bTooClose)
		{
			return Candidate;
		}
	}

	return Origin + FVector(
		FMath::FRandRange(-HalfExtent, HalfExtent),
		FMath::FRandRange(-HalfExtent, HalfExtent),
		SpawnHeight);
}

void AEnemySpawnerActor::HandleEnemyDied(AEnemyCubeActor* Enemy)
{
	AliveEnemies.Remove(Enemy);

	if (RespawnDelay <= 0.f)
	{
		EnsureEnemyCount();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&AEnemySpawnerActor::EnsureEnemyCount,
			RespawnDelay,
			false);
	}
}
