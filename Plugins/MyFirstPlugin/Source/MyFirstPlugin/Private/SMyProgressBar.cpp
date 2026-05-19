#include "SMyProgressBar.h"
#include "Widgets/InvalidateWidgetReason.h"

void SMyProgressBar::Construct(const FArguments& InArgs)
{
    Percent = InArgs._Percent;
    BarColor = InArgs._BarColor;
}

FVector2D SMyProgressBar::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return FVector2D(0.0f, 30.0f * LayoutScaleMultiplier);
}

int32 SMyProgressBar::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const float CurrentPercent = FMath::Clamp(Percent.Get(), 0.0f, 1.0f);
    const FLinearColor Color = BarColor.Get();

    const FVector2D Size = AllottedGeometry.GetLocalSize();

    // 背景
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform()),
        FAppStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.1f, 0.1f, 0.1f, 1.0f)
    );

    // 前景进度
    const float ProgressWidth = Size.X * CurrentPercent;
    if (ProgressWidth > 0.0f)
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(ProgressWidth, Size.Y),
                FSlateLayoutTransform()
            ),
            FAppStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            Color
        );
    }

    return LayerId + 1;
}

void SMyProgressBar::SetPercent(float InPercent)
{
    Percent.Set(InPercent);
    Invalidate(EInvalidateWidget::Paint);  // 将 LayoutAndPaint 改为 Paint
}