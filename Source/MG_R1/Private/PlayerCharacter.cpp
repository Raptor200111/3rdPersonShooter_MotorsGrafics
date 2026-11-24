#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Components/GunComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"
#include "UI/CrosshairWidget.h"
#include "UI/InteractPromptWidget.h"
#include "Interfaces/Interactuable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	ApplyCameraDefaults();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GunComponent = CreateDefaultSubobject<UGunComponent>(TEXT("GunComponent"));
	GunComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	FireSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FireSceneComponent"));
	FireSceneComponent->SetupAttachment(GunComponent);

	CrosshairWidgetClass = UCrosshairWidget::StaticClass();
	InteractPromptWidgetClass = UInteractPromptWidget::StaticClass();
}

void APlayerCharacter::ApplyCameraDefaults()
{
	if (!SpringArmComponent)
	{
		return;
	}

	SpringArmComponent->SetRelativeLocation(FVector::ZeroVector);
	SpringArmComponent->TargetArmLength = 650.f;
	SpringArmComponent->SocketOffset = FVector(0.f, 70.f, 55.f);
	SpringArmComponent->TargetOffset = FVector::ZeroVector;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->ProbeSize = 12.f;
	SpringArmComponent->ProbeChannel = ECC_Camera;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->bUseControllerDesiredRotation = false;
	}
	ApplyCameraDefaults();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;

		if (IsLocallyControlled())
		{
			if (CrosshairWidgetClass)
			{
				CrosshairWidget = CreateWidget<UUserWidget>(PC, CrosshairWidgetClass);
				if (CrosshairWidget)
				{
					CrosshairWidget->AddToViewport(100);
				}
			}
			if (InteractPromptWidgetClass)
			{
				InteractPromptWidget = CreateWidget<UUserWidget>(PC, InteractPromptWidgetClass);
				if (InteractPromptWidget)
				{
					InteractPromptWidget->AddToViewport(110);
					InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CrosshairWidget)
	{
		CrosshairWidget->RemoveFromParent();
		CrosshairWidget = nullptr;
	}
	if (InteractPromptWidget)
	{
		InteractPromptWidget->RemoveFromParent();
		InteractPromptWidget = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}
	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
	if (FireAction)
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::Fire);
	}
	if (ToggleWeaponDebugAction)
	{
		EnhancedInputComponent->BindAction(ToggleWeaponDebugAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleWeaponDebug);
	}

	PlayerInputComponent->BindKey(EKeys::T, IE_Pressed, this, &APlayerCharacter::ToggleWeaponDebug);
	PlayerInputComponent->BindKey(EKeys::P, IE_Pressed, this, &APlayerCharacter::TryInteract);
}

void APlayerCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookVector = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::ToggleWeaponDebug()
{
	if (GunComponent)
	{
		GunComponent->ToggleWeaponDebug();
	}
}

void APlayerCharacter::SetInteractPromptVisible(bool bVisible, AActor* Interactable)
{
	FocusedInteractable = (bVisible && Interactable) ? Interactable : nullptr;

	if (InteractPromptWidget)
	{
		InteractPromptWidget->SetVisibility(
			bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void APlayerCharacter::TryInteract()
{
	if (AActor* Focused = FocusedInteractable.Get())
	{
		if (Focused->GetClass()->ImplementsInterface(UInteractuable::StaticClass()))
		{
			IInteractuable::Execute_Interact(Focused, this);
			return;
		}
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector StartLocation;
	FRotator ViewRotation;
	GetActorEyesViewPoint(StartLocation, ViewRotation);
	if (CameraComponent)
	{
		StartLocation = CameraComponent->GetComponentLocation();
		ViewRotation = CameraComponent->GetComponentRotation();
	}

	const FVector EndLocation = StartLocation + ViewRotation.Vector() * InteractTraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

	if (GunComponent && GunComponent->IsWeaponDebugEnabled())
	{
		DrawDebugLine(World, StartLocation, EndLocation, FColor::Cyan, false, 0.5f, 0, 1.5f);
	}

	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;
	if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractuable::StaticClass()))
	{
		IInteractuable::Execute_Interact(HitActor, this);
	}
}

FVector APlayerCharacter::GetFireStartLocation(FVector& OutAimDirection) const
{
	FVector CamLoc = GetActorLocation() + FVector(0.f, 0.f, ChestFireHeight);
	OutAimDirection = GetActorForwardVector();

	if (CameraComponent)
	{
		CamLoc = CameraComponent->GetComponentLocation();
		OutAimDirection = CameraComponent->GetForwardVector();
	}
	else if (Controller)
	{
		const FRotator ControlRot = Controller->GetControlRotation();
		OutAimDirection = FRotator(ControlRot.Pitch, ControlRot.Yaw, 0.f).Vector();
	}

	OutAimDirection = OutAimDirection.GetSafeNormal();
	const FVector ChestPos = GetActorLocation() + FVector(0.f, 0.f, ChestFireHeight);
	const float Depth = FMath::Max(0.f, FVector::DotProduct(ChestPos - CamLoc, OutAimDirection));
	return CamLoc + OutAimDirection * Depth;
}

void APlayerCharacter::Fire()
{
	if (FireMontage)
	{
		PlayAnimMontage(FireMontage);
	}

	if (!GunComponent || !FireSceneComponent)
	{
		return;
	}

	FVector AimDirection;
	const FVector StartPos = GetFireStartLocation(AimDirection);
	FireSceneComponent->SetWorldLocation(StartPos);
	FireSceneComponent->SetWorldRotation(AimDirection.Rotation());
	GunComponent->Fire(FireSceneComponent);
}
