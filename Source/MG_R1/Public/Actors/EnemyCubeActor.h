#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCubeActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyCubeDied, AEnemyCubeActor*, Enemy);

UCLASS()
class MG_R1_API AEnemyCubeActor : public AActor
{
	GENERATED_BODY()

public:
	AEnemyCubeActor();

	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyCubeDied OnEnemyDied;

	bool IsAlive() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (ClampMin = "0.05"))
	float SpawnAnimDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (ClampMin = "0.05"))
	float DeathAnimDuration = 0.35f;

private:
	bool bIsDying = false;
	bool bSpawnAnimating = true;
	float AnimTime = 0.f;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BodyMID;

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleHealthDepleted();

	void UpdateHealthColor();
	void FinishDeath();
};
