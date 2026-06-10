#include "SFpsChart.h"
#include "Styling/AppStyle.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"

void SFpsChart::Construct(const FArguments& InArgs)
{
    MaxPoints = InArgs._MaxDataPoints;
    FrameTimes.Reserve(MaxPoints);
    CurrentIndex = 0;
    bIsFull = false;
}

FVector2D SFpsChart::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    // 宽度由父容器决定，高度固定200
    return FVector2D(0.0f, 200.0f * LayoutScaleMultiplier);
}

float SFpsChart::GetFpsFromDeltaTime(float DeltaTime) const
{
    return (DeltaTime > 0.0f) ? 1.0f / DeltaTime : 0.0f;
}

void SFpsChart::AddDataPoint(float DeltaTime)
{
    // 环形缓冲区：写入当前位置，然后前移索引
    if (FrameTimes.Num() < MaxPoints)
    {
        FrameTimes.Add(DeltaTime);
    }
    else
    {
        FrameTimes[CurrentIndex] = DeltaTime;
        bIsFull = true;
    }
    CurrentIndex = (CurrentIndex + 1) % MaxPoints;

    // 强制重绘
    Invalidate(EInvalidateWidget::Paint);
}

int32 SFpsChart::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D Size = AllottedGeometry.GetLocalSize();
    if (Size.X <= 0 || Size.Y <= 0) return LayerId;

    const float Margin = 30.0f;   // 边距，留出文字空间
    const float GraphLeft = Margin;
    const float GraphRight = Size.X - Margin;
    const float GraphTop = Margin;
    const float GraphBottom = Size.Y - Margin * 1.5f;
    const float GraphWidth = GraphRight - GraphLeft;
    const float GraphHeight = GraphBottom - GraphTop;

    // ---------- 绘制背景 ----------
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform()),
        FAppStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.05f, 0.05f, 0.05f, 1.0f) // 深灰背景
    );

    if (FrameTimes.Num() == 0)
    {
        // 没有数据时，绘制“No Data”文本
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(FVector2D(100, 20), FSlateLayoutTransform(FVector2D(Margin, Margin))),
            TEXT("No data..."),
            FCoreStyle::GetDefaultFontStyle("Regular", 12),
            ESlateDrawEffect::None,
            FLinearColor::Gray
        );
        return LayerId + 1;
    }

    // ---------- 绘制参考线（30FPS / 60FPS） ----------
    auto DrawHorizontalDash = [&](float Y, FLinearColor Color)
        {
            const float DashLength = 5.0f;
            for (float X = GraphLeft; X < GraphRight; X += DashLength * 2)
            {
                FSlateDrawElement::MakeBox(
                    OutDrawElements,
                    LayerId + 1,
                    AllottedGeometry.ToPaintGeometry(
                        FVector2D(DashLength, 1.0f),
                        FSlateLayoutTransform(FVector2D(X, Y))
                    ),
                    FAppStyle::Get().GetBrush("WhiteBrush"),
                    ESlateDrawEffect::None,
                    Color
                );
            }
        };

    // 30FPS = 33.33ms, 60FPS = 16.67ms
    // 假设最大帧时间 50ms，映射到 GraphHeight
    const float MaxFrameTime = 0.050f; // 50ms
    const float Fps30Y = GraphTop + (0.03333f / MaxFrameTime) * GraphHeight;
    const float Fps60Y = GraphTop + (0.01667f / MaxFrameTime) * GraphHeight;
    DrawHorizontalDash(Fps30Y, FLinearColor(1.0f, 1.0f, 0.0f, 0.5f)); // 黄色虚线
    DrawHorizontalDash(Fps60Y, FLinearColor(0.0f, 1.0f, 0.0f, 0.5f)); // 绿色虚线

    // ---------- 构建折线数据 ----------
    // 从最旧的数据点到最新的数据点
    TArray<FVector2D> LinePoints;
    int32 Count = bIsFull ? MaxPoints : FrameTimes.Num();
    int32 StartIndex = bIsFull ? CurrentIndex : 0; // 最旧数据索引
    for (int32 i = 0; i < Count; ++i)
    {
        int32 idx = (StartIndex + i) % MaxPoints;
        float DeltaTime = FrameTimes[idx];
        float X = GraphLeft + (float)i / (Count - 1) * GraphWidth;
        float Y = GraphTop + (DeltaTime / MaxFrameTime) * GraphHeight;
        Y = FMath::Clamp(Y, GraphTop, GraphBottom);
        LinePoints.Add(FVector2D(X, Y));
    }

    // ---------- 绘制折线 ----------
    if (LinePoints.Num() >= 2)
    {
        FSlateDrawElement::MakeLines(
            OutDrawElements,
            LayerId + 2,
            AllottedGeometry.ToPaintGeometry(),
            LinePoints,
            ESlateDrawEffect::None,
            FLinearColor::Green,
            true,   // bAntialias
            2.0f    // 线宽
        );
    }

    // ---------- 绘制当前帧率文本 ----------
    float LatestDeltaTime = FrameTimes[(CurrentIndex - 1 + MaxPoints) % MaxPoints];
    float CurrentFps = GetFpsFromDeltaTime(LatestDeltaTime);
    FString FpsText = FString::Printf(TEXT("FPS: %.1f"), CurrentFps);
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId + 3,
        AllottedGeometry.ToPaintGeometry(
            FVector2D(100, 20),
            FSlateLayoutTransform(FVector2D(GraphRight - 100, GraphTop - 20))
        ),
        FpsText,
        FCoreStyle::GetDefaultFontStyle("Bold", 12),
        ESlateDrawEffect::None,
        FLinearColor::White
    );

    return LayerId + 3;
}