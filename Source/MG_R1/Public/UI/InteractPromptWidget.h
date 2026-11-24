#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractPromptWidget.generated.h"

UCLASS()
class MG_R1_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPromptText(const FText& InText);

protected:
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

private:
	FText PromptText = FText::FromString(TEXT("[P] Abrir / cerrar puerta"));
};
