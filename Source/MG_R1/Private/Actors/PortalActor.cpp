#include "Actors/PortalActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(Root);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PortalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PortalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PortalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	PortalMesh->SetRelativeScale3D(FVector(0.15f, 2.5f, 3.0f));
	PortalMesh->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		PortalMesh->SetStaticMesh(CylinderMesh.Object);
	}

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetBoxExtent(FVector(60.f, 120.f, 160.f));
	TriggerBox->SetRelativeLocation(FVector(40.f, 0.f, 100.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	ExitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPoint"));
	ExitPoint->SetupAttachment(Root);
	ExitPoint->SetRelativeLocation(FVector(120.f, 0.f, 0.f));
}

void APortalActor::BeginPlay()
{
	Super::BeginPlay();

	PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PortalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PortalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortalActor::OnTriggerBeginOverlap);
}

void APortalActor::MarkCooldown(AActor* Traveler)
{
	if (Traveler)
	{
		RecentlyTeleported.Add(Traveler);
	}
}

void APortalActor::ClearCooldown(AActor* Traveler)
{
	RecentlyTeleported.Remove(Traveler);
}

bool APortalActor::IsValidTraveler(const AActor* OtherActor) const
{
	return OtherActor && OtherActor != this && OtherActor->IsA<APawn>();
}

FVector APortalActor::ResolveDestinationLocation() const
{
	if (!Destination)
	{
		return GetActorLocation();
	}

	if (const APortalActor* DestPortal = Cast<APortalActor>(Destination))
	{
		if (DestPortal->GetExitPoint())
		{
			return DestPortal->GetExitPoint()->GetComponentLocation();
		}
	}

	return Destination->GetActorLocation();
}

FRotator APortalActor::ResolveDestinationRotation() const
{
	if (!Destination)
	{
		return GetActorRotation();
	}

	if (const APortalActor* DestPortal = Cast<APortalActor>(Destination))
	{
		if (DestPortal->GetExitPoint())
		{
			return DestPortal->GetExitPoint()->GetComponentRotation();
		}
	}

	return Destination->GetActorRotation();
}

void APortalActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidTraveler(OtherActor) || !Destination || RecentlyTeleported.Contains(OtherActor))
	{
		return;
	}

	const FRotator DestRot = ResolveDestinationRotation();
	const FVector DestLoc = ResolveDestinationLocation();

	MarkCooldown(OtherActor);
	if (APortalActor* DestPortal = Cast<APortalActor>(Destination))
	{
		DestPortal->MarkCooldown(OtherActor);
	}

	OtherActor->TeleportTo(DestLoc, DestRot, false, true);

	if (APawn* TravelerPawn = Cast<APawn>(OtherActor))
	{
		if (AController* PC = TravelerPawn->GetController())
		{
			PC->SetControlRotation(DestRot);
		}
	}

	TWeakObjectPtr<AActor> WeakTraveler(OtherActor);
	TWeakObjectPtr<APortalActor> WeakDest(Cast<APortalActor>(Destination));
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(
		Handle,
		FTimerDelegate::CreateLambda([this, WeakTraveler, WeakDest]()
		{
			if (AActor* Traveler = WeakTraveler.Get())
			{
				ClearCooldown(Traveler);
				if (APortalActor* Dest = WeakDest.Get())
				{
					Dest->ClearCooldown(Traveler);
				}
			}
		}),
		TeleportCooldown,
		false);
}
