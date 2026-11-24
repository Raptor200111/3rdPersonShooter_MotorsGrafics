#include "UI/CrosshairWidget.h"
#include "Rendering/DrawElements.h"

int32 UCrosshairWidget::NativePaint(
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
	const FVector2D Center = Size * 0.5f;
	constexpr float Arm = 12.f;
	constexpr float Gap = 4.f;
	constexpr float Thickness = 2.f;

	TArray<FVector2D> HLeft = { FVector2D(Center.X - Gap - Arm, Center.Y), FVector2D(Center.X - Gap, Center.Y) };
	TArray<FVector2D> HRight = { FVector2D(Center.X + Gap, Center.Y), FVector2D(Center.X + Gap + Arm, Center.Y) };
	TArray<FVector2D> VTop = { FVector2D(Center.X, Center.Y - Gap - Arm), FVector2D(Center.X, Center.Y - Gap) };
	TArray<FVector2D> VBottom = { FVector2D(Center.X, Center.Y + Gap), FVector2D(Center.X, Center.Y + Gap + Arm) };

	const FLinearColor Color = FLinearColor::White;
	auto DrawSeg = [&](const TArray<FVector2D>& Points)
	{
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			RetLayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			Points,
			ESlateDrawEffect::None,
			Color,
			true,
			Thickness);
	};

	DrawSeg(HLeft);
	DrawSeg(HRight);
	DrawSeg(VTop);
	DrawSeg(VBottom);

	return RetLayerId + 1;
}
