#include "MyCharacterAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void UMyCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	if (!IsValid(Pawn))
	{
		return;
	}

	CharacterMovementComponent = Pawn->GetComponentByClass<UCharacterMovementComponent>();
	PlayerCharacter = Cast<APlayerCharacter>(Pawn);
}

void UMyCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(CharacterMovementComponent) || !IsValid(PlayerCharacter))
	{
		return;
	}

	const FVector Velocity = CharacterMovementComponent->Velocity;
	Speed = Velocity.Size2D();
	bIsMoving = Speed > 10.f;
	ZVelocity = Velocity.Z;
	bIsFalling = CharacterMovementComponent->IsFalling();
	AimPitch = -PlayerCharacter->GetControlRotation().Pitch;
}
