#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GunComponent.generated.h"

class UNiagaraSystem;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MG_R1_API UGunComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UGunComponent();

	float GetDistance() const { return Distance; }

	bool IsWeaponDebugEnabled() const { return bWeaponDebug; }
	void SetWeaponDebugEnabled(bool bEnabled);
	void ToggleWeaponDebug();

	void Fire(USceneComponent* FirePosComponent);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float Distance = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", meta = (ClampMin = "0.0"))
	float DamagePerShot = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float DebugDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ImpactFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bWeaponDebug = false;
};
