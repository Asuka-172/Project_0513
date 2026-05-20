#include "SColorWheel.h"
#include "InputCoreTypes.h"
#include "Styling/AppStyle.h"

void SColorWheel::Construct(const FArguments& InArgs)
{
    OnColorChangedDelegate = InArgs._OnColorChanged;
    CurrentColor = FLinearColor::Red; // 初始：红色
}

FVector2D SColorWheel::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    // 宽度 200 像素，高度 30 像素（根据缩放调整）
    return FVector2D(200.0f, 30.0f) * LayoutScaleMultiplier;
}

FLinearColor SColorWheel::GetColorFromLocalPos(const FGeometry& MyGeometry, const FVector2D& LocalPos) const
{
    const float Width = MyGeometry.GetLocalSize().X;
    // 将水平位置映射到色相 (0.0 ~ 1.0)
    float Hue = FMath::Clamp(LocalPos.X / Width, 0.0f, 1.0f);
    // 固定饱和度和明度为最大，从 HSV 生成颜色
    return FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue * 255.0f), 255, 255);
}

void SColorWheel::UpdateColor(const FGeometry& MyGeometry, const FVector2D& LocalPos)
{
    FLinearColor NewColor = GetColorFromLocalPos(MyGeometry, LocalPos);
    if (!NewColor.Equals(CurrentColor))
    {
        CurrentColor = NewColor;
        // 通知外部颜色已改变
        OnColorChangedDelegate.ExecuteIfBound(CurrentColor);
        // 触发重绘，更新指示器位置
        Invalidate(EInvalidateWidget::Paint);
    }
}

int32 SColorWheel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D Size = AllottedGeometry.GetLocalSize();
    const int32 NumSegments = 50;              // 色带分段数
    const float SegmentWidth = Size.X / NumSegments;

    // 绘制色相条（多个竖条渐变）
    for (int32 i = 0; i < NumSegments; ++i)
    {
        float Hue = static_cast<float>(i) / NumSegments;
        FLinearColor SegmentColor = FLinearColor::MakeFromHSV8(
            static_cast<uint8>(Hue * 255.0f), 255, 255);

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(SegmentWidth, Size.Y),
                FSlateLayoutTransform(FVector2D(i * SegmentWidth, 0))
            ),
            FAppStyle::Get().GetBrush("WhiteBrush"),
            ESlateDrawEffect::None,
            SegmentColor
        );
    }

    // 计算当前颜色对应的指示器位置（基于色相）
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();
    float CurrentHue = HSV.R;  // 在 UE 的 HSV 表示中，H 存放在 R 通道
    float IndicatorX = CurrentHue * Size.X;

    // 绘制指示器（一个细长的黑色竖条）
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(
            FVector2D(2.0f, Size.Y),
            FSlateLayoutTransform(FVector2D(IndicatorX - 1.0f, 0))
        ),
        FAppStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor::Black
    );

    return LayerId + 1;
}

FReply SColorWheel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UE_LOG(LogTemp, Warning, TEXT("SColorWheel::OnMouseButtonDown called"));

    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        UpdateColor(MyGeometry, LocalPos);
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }
    return FReply::Unhandled();
}

FReply SColorWheel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UE_LOG(LogTemp, Warning, TEXT("SColorWheel::OnMouseMove called"));

    // 左键按住时更新颜色
    if (HasMouseCapture() && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        UpdateColor(MyGeometry, LocalPos);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}