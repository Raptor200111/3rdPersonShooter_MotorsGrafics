#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UHealthComponent::GetHealthPercent() const
{
	return MaxHealth > 0.f ? FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f) : 0.f;
}

float UHealthComponent::ApplyDamage(float Amount)
{
	if (!IsAlive() || Amount <= 0.f)
	{
		return 0.f;
	}

	const float Old = CurrentHealth;
	CurrentHealth = FMath::Max(0.f, CurrentHealth - Amount);
	const float Applied = Old - CurrentHealth;

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.f)
	{
		OnHealthDepleted.Broadcast();
	}

	return Applied;
}
