#include "SAssetPieChart.h"
#include "Styling/AppStyle.h"
#include "Fonts/FontMeasure.h"

// 预设一组颜色
const TArray<FLinearColor> SAssetPieChart::DefaultColors = {
    FLinearColor(0.1f, 0.5f, 0.9f, 1.0f),  // 蓝
    FLinearColor(0.9f, 0.3f, 0.2f, 1.0f),  // 红
    FLinearColor(0.1f, 0.8f, 0.4f, 1.0f),  // 绿
    FLinearColor(0.9f, 0.7f, 0.1f, 1.0f),  // 黄
    FLinearColor(0.6f, 0.3f, 0.9f, 1.0f),  // 紫
    FLinearColor(0.0f, 0.7f, 0.7f, 1.0f),  // 青
};

void SAssetPieChart::Construct(const FArguments& InArgs)
{
    Slices = InArgs._Slices;
}

FVector2D SAssetPieChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    // 宽度由父容器决定，高度固定 200
    return FVector2D(0.0f, 200.0f * LayoutScaleMultiplier);
}

void SAssetPieChart::SetSlices(const TArray<FSliceData>& InSlices)
{
    Slices = InSlices;
    Invalidate(EInvalidateWidget::Paint);
}

int32 SAssetPieChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D Size = AllottedGeometry.GetLocalSize();
    if (Size.X <= 0 || Size.Y <= 0) return LayerId;

    // 计算总数
    int32 Total = 0;
    for (const FSliceData& Slice : Slices)
        Total += Slice.Count;

    if (Total == 0) return LayerId;

    // 布局参数
    const float LeftMargin = 10.0f;
    const float BarAreaWidth = Size.X * 0.55f;   // 左侧柱状图占 55%
    const float BarWidth = BarAreaWidth / FMath::Max(1, Slices.Num());
    const float MaxBarHeight = Size.Y - 40.0f;    // 顶部留 20，底部留 20 给标签
    const float BarBottom = Size.Y - 20.0f;

    // 绘制背景
    FSlateDrawElement::MakeBox(
        OutDrawElements, LayerId,
        AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform()),
        FAppStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.06f, 0.06f, 0.06f, 1.0f)
    );

    // 绘制每个柱状条和图例
    for (int32 i = 0; i < Slices.Num(); ++i)
    {
        float Ratio = (float)Slices[i].Count / Total;
        float BarHeight = Ratio * MaxBarHeight;
        FLinearColor Color = Slices[i].Color.IsAlmostBlack()
            ? DefaultColors[i % DefaultColors.Num()] : Slices[i].Color;

        // 柱状条
        float BarX = LeftMargin + i * BarWidth;
        float BarTop = BarBottom - BarHeight;
        FSlateDrawElement::MakeBox(
            OutDrawElements, LayerId + 1,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(BarWidth - 4.0f, BarHeight),
                FSlateLayoutTransform(FVector2D(BarX + 2.0f, BarTop))
            ),
            FAppStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            Color
        );

        // 柱状条下方的类型名
        FSlateDrawElement::MakeText(
            OutDrawElements, LayerId + 2,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(BarWidth, 16.0f),
                FSlateLayoutTransform(FVector2D(BarX, BarBottom + 2.0f))
            ),
            Slices[i].Label,
            FCoreStyle::GetDefaultFontStyle("Regular", 7),
            ESlateDrawEffect::None,
            FLinearColor::White
        );

        // 右侧图例：颜色方块 + 类型名 + 数量
        float LegendX = LeftMargin + BarAreaWidth + 20.0f;
        float LegendY = 30.0f + i * 25.0f;

        // 图例色块
        FSlateDrawElement::MakeBox(
            OutDrawElements, LayerId + 1,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(14.0f, 14.0f),
                FSlateLayoutTransform(FVector2D(LegendX, LegendY))
            ),
            FAppStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            Color
        );

        // 图例文字
        FString LegendText = FString::Printf(TEXT("%s: %d"), *Slices[i].Label, Slices[i].Count);
        FSlateDrawElement::MakeText(
            OutDrawElements, LayerId + 2,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(200.0f, 18.0f),
                FSlateLayoutTransform(FVector2D(LegendX + 18.0f, LegendY))
            ),
            LegendText,
            FCoreStyle::GetDefaultFontStyle("Regular", 9),
            ESlateDrawEffect::None,
            FLinearColor::White
        );
    }

    return LayerId + 3;
}