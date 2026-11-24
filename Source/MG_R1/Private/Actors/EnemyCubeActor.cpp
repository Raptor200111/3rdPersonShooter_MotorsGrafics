#include "Actors/EnemyCubeActor.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AEnemyCubeActor::AEnemyCubeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetRelativeScale3D(FVector(1.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (ShapeMat.Succeeded())
	{
		Mesh->SetMaterial(0, ShapeMat.Object);
	}

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

bool AEnemyCubeActor::IsAlive() const
{
	return !bIsDying && HealthComponent && HealthComponent->IsAlive();
}

void AEnemyCubeActor::BeginPlay()
{
	Super::BeginPlay();

	BodyMID = Mesh->CreateDynamicMaterialInstance(0);
	UpdateHealthColor();

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCubeActor::HandleHealthChanged);
		HealthComponent->OnHealthDepleted.AddDynamic(this, &AEnemyCubeActor::HandleHealthDepleted);
	}

	SetActorScale3D(FVector(0.01f));
	bSpawnAnimating = true;
	AnimTime = 0.f;
}

void AEnemyCubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSpawnAnimating)
	{
		AnimTime += DeltaTime;
		const float Alpha = FMath::Clamp(AnimTime / SpawnAnimDuration, 0.f, 1.f);
		const float Scale = FMath::InterpEaseOut(0.01f, 1.f, Alpha, 2.f);
		SetActorScale3D(FVector(Scale));
		if (Alpha >= 1.f)
		{
			bSpawnAnimating = false;
			SetActorScale3D(FVector(1.f));
		}
		return;
	}

	if (bIsDying)
	{
		AnimTime += DeltaTime;
		const float Alpha = FMath::Clamp(AnimTime / DeathAnimDuration, 0.f, 1.f);
		const float Scale = FMath::InterpEaseIn(1.f, 0.01f, Alpha, 2.f);
		SetActorScale3D(FVector(Scale));
		if (Alpha >= 1.f)
		{
			FinishDeath();
		}
	}
}

float AEnemyCubeActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsDying || Actual <= 0.f || !HealthComponent)
	{
		return 0.f;
	}

	return HealthComponent->ApplyDamage(Actual);
}

void AEnemyCubeActor::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	UpdateHealthColor();
}

void AEnemyCubeActor::HandleHealthDepleted()
{
	if (bIsDying)
	{
		return;
	}

	bIsDying = true;
	AnimTime = 0.f;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyCubeActor::UpdateHealthColor()
{
	if (!BodyMID || !HealthComponent)
	{
		return;
	}

	const float T = 1.f - HealthComponent->GetHealthPercent();
	const FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor::Green, FLinearColor::Red, T);
	BodyMID->SetVectorParameterValue(TEXT("Color"), Color);
}

void AEnemyCubeActor::FinishDeath()
{
	OnEnemyDied.Broadcast(this);
	Destroy();
}
