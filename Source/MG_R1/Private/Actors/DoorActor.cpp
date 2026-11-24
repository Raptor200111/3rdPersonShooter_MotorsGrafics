#include "Actors/DoorActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	HingeRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HingeRoot"));
	SetRootComponent(HingeRoot);

	DoorLeaf = CreateDefaultSubobject<USceneComponent>(TEXT("DoorLeaf"));
	DoorLeaf->SetupAttachment(HingeRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorLeaf);
	DoorMesh->SetMobility(EComponentMobility::Movable);
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionObjectType(ECC_WorldDynamic);
	DoorMesh->SetCollisionResponseToAllChannels(ECR_Block);
	DoorMesh->SetRelativeLocation(FVector(0.f, 50.f, 0.f));
	DoorMesh->SetRelativeScale3D(FVector(0.2f, 2.f, 2.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(HingeRoot);
	TriggerBox->SetBoxExtent(FVector(80.f, 120.f, 140.f));
	TriggerBox->SetRelativeLocation(FVector(0.f, 100.f, 100.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	ClosedYaw = DoorLeaf->GetRelativeRotation().Yaw;
	CurrentYaw = ClosedYaw;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::OnTriggerEndOverlap);
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDoorRotation(DeltaTime);
}

void ADoorActor::Interact_Implementation(AActor* Interactor)
{
	(void)Interactor;
	bShouldBeOpen = !bShouldBeOpen;
}

bool ADoorActor::IsValidOpener(const AActor* OtherActor) const
{
	return OtherActor && OtherActor != this && OtherActor->IsA<APawn>();
}

void ADoorActor::NotifyPrompt(AActor* OtherActor, bool bShow)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->SetInteractPromptVisible(bShow, bShow ? this : nullptr);
	}
}

void ADoorActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOpener(OtherActor))
	{
		return;
	}
	NotifyPrompt(OtherActor, true);
}

void ADoorActor::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValidOpener(OtherActor))
	{
		return;
	}
	NotifyPrompt(OtherActor, false);
}

void ADoorActor::UpdateDoorRotation(float DeltaTime)
{
	const float TargetYaw = bShouldBeOpen ? (ClosedYaw + OpenAngle) : ClosedYaw;
	CurrentYaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, OpenSpeed);
	DoorLeaf->SetRelativeRotation(FRotator(0.f, CurrentYaw, 0.f));
}
