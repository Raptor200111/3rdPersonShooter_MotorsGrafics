#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USceneComponent;
class UGunComponent;
class UHealthComponent;
class AProjectileActor;
class UAnimMontage;
class UUserWidget;

UCLASS()
class MG_R1_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UCameraComponent* GetPlayerCameraComponent() const { return CameraComponent; }

	void SetInteractPromptVisible(bool bVisible, AActor* Interactable = nullptr);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UGunComponent> GunComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> FireSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleWeaponDebugAction;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AProjectileActor> ProjectileActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ChestFireHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InteractPromptWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Interact", meta = (ClampMin = "50.0"))
	float InteractTraceDistance = 1200.f;

	UPROPERTY()
	TObjectPtr<UUserWidget> CrosshairWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> InteractPromptWidget;

	UPROPERTY()
	TWeakObjectPtr<AActor> FocusedInteractable;

	void ApplyCameraDefaults();
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Fire();
	void ToggleWeaponDebug();
	void TryInteract();
	FVector GetFireStartLocation(FVector& OutAimDirection) const;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
