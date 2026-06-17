#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"

class SAssetPieChart : public SLeafWidget
{
public:
    struct FSliceData
    {
        FString Label;
        int32 Count;
        FLinearColor Color;
    };

    // 手动定义 FArguments
    struct FArguments : public TSlateBaseNamedArgs<SAssetPieChart>
    {
        typedef FArguments WidgetArgsType;
        TArray<FSliceData> _Slices;
    };

    void Construct(const FArguments& InArgs);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

    // 设置数据
    void SetSlices(const TArray<FSliceData>& InSlices);

private:
    TArray<FSliceData> Slices;

    // 预设颜色
    static const TArray<FLinearColor> DefaultColors;
};