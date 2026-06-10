#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "SFpsChart.h"

class SPerformancePanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPerformancePanel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // 由定时器调用，更新所有显示数据
    void UpdateStats(float DeltaTime);

private:
    TSharedPtr<STextBlock> FpsTextBlock;
    TSharedPtr<STextBlock> MemoryTextBlock;
    TSharedPtr<STextBlock> DrawCallTextBlock;
    TSharedPtr<SFpsChart> FpsChart;

    // 文本颜色辅助函数
    FSlateColor GetFpsColor(float Fps) const;
    FSlateColor GetMemoryColor(float UsedGB, float TotalGB) const;
    FString FormatMemorySize(uint64 Bytes) const;
};