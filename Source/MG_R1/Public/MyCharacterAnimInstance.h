#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class APlayerCharacter;

UCLASS()
class MG_R1_API UMyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float ZVelocity = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float AimPitch = 0.f;
};
