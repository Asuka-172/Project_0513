#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

// 委托：当颜色改变时触发
DECLARE_DELEGATE_OneParam(FOnColorChanged, FLinearColor);

class SColorWheel : public SLeafWidget
{
public:
    // 手动定义 FArguments，避免 SLATE_BEGIN_ARGS 在插件中的兼容性问题
    struct FArguments : public TSlateBaseNamedArgs<SColorWheel>
    {
        typedef FArguments WidgetArgsType;

        FArguments& OnColorChanged(const FOnColorChanged& InDelegate)
        {
            _OnColorChanged = InDelegate;
            return *this;
        }

        FOnColorChanged _OnColorChanged;
    };

    void Construct(const FArguments& InArgs);

    // 覆写绘制
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    // 返回期望尺寸
    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

    // 鼠标事件
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    // 根据控件上的本地坐标计算对应的颜色
    FLinearColor GetColorFromLocalPos(const FGeometry& MyGeometry, const FVector2D& LocalPos) const;
    // 更新当前颜色并触发委托
    void UpdateColor(const FGeometry& MyGeometry, const FVector2D& LocalPos);

    FLinearColor CurrentColor;
    FOnColorChanged OnColorChangedDelegate;
};