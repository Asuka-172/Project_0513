#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"

class SMyProgressBar : public SLeafWidget
{
public:
    struct FArguments : public TSlateBaseNamedArgs<SMyProgressBar>
    {
        typedef FArguments WidgetArgsType;

        FArguments& Percent(const TAttribute<float>& InPercent)
        {
            _Percent = InPercent;
            return *this;
        }

        FArguments& BarColor(const TAttribute<FLinearColor>& InColor)
        {
            _BarColor = InColor;
            return *this;
        }

        TAttribute<float> _Percent;
        TAttribute<FLinearColor> _BarColor;
    };

    void Construct(const FArguments& InArgs);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

    void SetPercent(float InPercent);

private:
    TAttribute<float> Percent;
    TAttribute<FLinearColor> BarColor;
};