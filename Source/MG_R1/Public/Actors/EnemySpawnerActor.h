#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnerActor.generated.h"

class AEnemyCubeActor;
class UBoxComponent;

UCLASS()
class MG_R1_API AEnemySpawnerActor : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawnerActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> AreaPreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "1"))
	int32 MaxAlive = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "1.0"))
	float HalfExtent = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AEnemyCubeActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0.0"))
	float RespawnDelay = 0.4f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AEnemyCubeActor>> AliveEnemies;

	void EnsureEnemyCount();
	void SpawnOneEnemy();
	FVector PickSpawnLocation() const;

	UFUNCTION()
	void HandleEnemyDied(AEnemyCubeActor* Enemy);

	FTimerHandle RespawnTimerHandle;
};
