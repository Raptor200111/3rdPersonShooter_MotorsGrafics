#include "UI/InteractPromptWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void UInteractPromptWidget::SetPromptText(const FText& InText)
{
	PromptText = InText;
}

int32 UInteractPromptWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	const int32 RetLayerId = Super::NativePaint(
		Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FVector2D TextPos(Size.X * 0.5f - 140.f, Size.Y * 0.72f);

	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Bold", 22);

	FSlateDrawElement::MakeText(
		OutDrawElements,
		RetLayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(320.f, 40.f), FSlateLayoutTransform(TextPos)),
		PromptText,
		Font,
		ESlateDrawEffect::None,
		FLinearColor(1.f, 0.92f, 0.35f, 1.f));

	return RetLayerId + 1;
}
