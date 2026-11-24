#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatformActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class MG_R1_API AMovingPlatformActor : public AActor
{
	GENERATED_BODY()

public:
	AMovingPlatformActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	TObjectPtr<AActor> PointA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	TObjectPtr<AActor> PointB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MaxSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MinSpeed = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float Acceleration = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.0"))
	float TurnDistance = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float SlowDownDistance = 0.f;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

	float CurrentSpeed = 0.f;

	AActor* GetOtherPoint(const AActor* Point) const;
	FVector GetPathTargetLocation() const;
};
