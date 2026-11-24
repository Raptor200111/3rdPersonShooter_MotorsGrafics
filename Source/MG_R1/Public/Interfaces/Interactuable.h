#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactuable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractuable : public UInterface
{
	GENERATED_BODY()
};

class MG_R1_API IInteractuable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void Interact(AActor* Interactor);
};
