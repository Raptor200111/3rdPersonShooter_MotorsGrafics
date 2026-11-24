#include "Components/GunComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"

UGunComponent::UGunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DefaultImpact(
		TEXT("/Game/Variant_Combat/VFX/NS_Damage.NS_Damage"));
	if (DefaultImpact.Succeeded())
	{
		ImpactFX = DefaultImpact.Object;
	}
}

void UGunComponent::SetWeaponDebugEnabled(bool bEnabled)
{
	bWeaponDebug = bEnabled;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			99101,
			2.0f,
			bWeaponDebug ? FColor::Cyan : FColor::Silver,
			bWeaponDebug ? TEXT("Weapon debug ON") : TEXT("Weapon debug OFF"));
	}
}

void UGunComponent::ToggleWeaponDebug()
{
	SetWeaponDebugEnabled(!bWeaponDebug);
}

void UGunComponent::Fire(USceneComponent* FirePosComponent)
{
	if (!FirePosComponent || !GetWorld())
	{
		return;
	}

	FHitResult OutHit;
	const FVector StartPos = FirePosComponent->GetComponentLocation();
	const FVector EndPos = StartPos + FirePosComponent->GetForwardVector() * Distance;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetOwner());

	if (bWeaponDebug)
	{
		DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Green, false, DebugDuration, 0, 3.0f);
		DrawDebugSphere(GetWorld(), StartPos, 8.0f, 8, FColor::Yellow, false, DebugDuration);
	}

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHit,
		StartPos,
		EndPos,
		ECC_Visibility,
		CollisionQueryParams);

	if (bHit && OutHit.bBlockingHit)
	{
		if (AActor* HitActor = OutHit.GetActor())
		{
			AController* InstigatorController = nullptr;
			if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
			{
				InstigatorController = OwnerPawn->GetController();
			}

			UGameplayStatics::ApplyPointDamage(
				HitActor,
				DamagePerShot,
				FirePosComponent->GetForwardVector(),
				OutHit,
				InstigatorController,
				GetOwner(),
				UDamageType::StaticClass());
		}

		if (ImpactFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				ImpactFX,
				OutHit.ImpactPoint,
				OutHit.ImpactNormal.Rotation(),
				FVector(1.f),
				true,
				true);
		}

		if (bWeaponDebug)
		{
			DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 12.0f, 8, FColor::Green, false, DebugDuration);
			if (GEngine && OutHit.GetActor())
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					DebugDuration,
					FColor::Orange,
					FString::Printf(TEXT("Hitting Actor: %s, Impact Point: %s"),
						*OutHit.GetActor()->GetName(),
						*OutHit.ImpactPoint.ToString()));
			}
		}
	}
}
