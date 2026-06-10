#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"

class SFpsChart : public SLeafWidget
{
public:
    // 手动定义 FArguments
    struct FArguments : public TSlateBaseNamedArgs<SFpsChart>
    {
        typedef FArguments WidgetArgsType;

        FArguments& MaxDataPoints(int32 InMaxDataPoints)
        {
            _MaxDataPoints = InMaxDataPoints;
            return *this;
        }

        int32 _MaxDataPoints = 200;
    };

    void Construct(const FArguments& InArgs);
    void AddDataPoint(float DeltaTime);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
    float GetFpsFromDeltaTime(float DeltaTime) const;

    TArray<float> FrameTimes;
    int32 MaxPoints = 200;
    int32 CurrentIndex = 0;
    bool bIsFull = false;
};