#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MG_R1_API APortalActor : public AActor
{
	GENERATED_BODY()

public:
	APortalActor();

	USceneComponent* GetExitPoint() const { return ExitPoint; }

	void MarkCooldown(AActor* Traveler);
	void ClearCooldown(AActor* Traveler);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PortalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ExitPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	TObjectPtr<AActor> Destination;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal", meta = (ClampMin = "0.05"))
	float TeleportCooldown = 1.0f;

private:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> RecentlyTeleported;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsValidTraveler(const AActor* OtherActor) const;
	FVector ResolveDestinationLocation() const;
	FRotator ResolveDestinationRotation() const;
};
