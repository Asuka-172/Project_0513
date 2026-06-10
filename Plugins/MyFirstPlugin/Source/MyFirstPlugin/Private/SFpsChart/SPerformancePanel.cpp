#include "SPerformancePanel.h"
#include "Widgets/Layout/SBox.h"
#include "HAL/PlatformMemory.h"
#include "RHI.h"

void SPerformancePanel::Construct(const FArguments& InArgs)
{
    ChildSlot
        [
            SNew(SVerticalBox)

                // ----- FPS 和帧时间 -----
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                [
                    SAssignNew(FpsTextBlock, STextBlock)
                        .Text(FText::FromString("FPS: --"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                ]

                // ----- 内存使用 -----
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                [
                    SAssignNew(MemoryTextBlock, STextBlock)
                        .Text(FText::FromString("Memory: --"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                ]

                // ----- Draw Call 数量 -----
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                [
                    SAssignNew(DrawCallTextBlock, STextBlock)
                        .Text(FText::FromString("Draw Calls: --"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
                ]

                // ----- 帧率图表 -----
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                [
                    SAssignNew(FpsChart, SFpsChart)
                        .MaxDataPoints(200)
                ]
        ];

    // 初始化内存统计
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    // 初次显示
    UpdateStats(0.016f);
}

void SPerformancePanel::UpdateStats(float DeltaTime)
{
    // 1. FPS 和帧时间
    float Fps = (DeltaTime > 0.0f) ? 1.0f / DeltaTime : 0.0f;
    float FrameTimeMs = DeltaTime * 1000.0f;
    FString FpsStr = FString::Printf(TEXT("FPS: %.1f  (%.2f ms)"), Fps, FrameTimeMs);
    FpsTextBlock->SetText(FText::FromString(FpsStr));
    FpsTextBlock->SetColorAndOpacity(GetFpsColor(Fps));

    // 2. 内存使用
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    uint64 UsedPhysical = MemStats.UsedPhysical;
    uint64 AvailablePhysical = MemStats.AvailablePhysical;
    uint64 TotalPhysical = UsedPhysical + AvailablePhysical;
    float UsedGB = (float)UsedPhysical / (1024.0f * 1024.0f * 1024.0f);
    float TotalGB = (float)TotalPhysical / (1024.0f * 1024.0f * 1024.0f);
    FString MemStr = FString::Printf(TEXT("Memory: %.2f GB / %.2f GB"), UsedGB, TotalGB);
    MemoryTextBlock->SetText(FText::FromString(MemStr));
    MemoryTextBlock->SetColorAndOpacity(GetMemoryColor(UsedGB, TotalGB));

    // 3. Draw Call
    int32 DrawCalls = GNumDrawCallsRHI[0];
    FString DrawCallStr = FString::Printf(TEXT("Draw Calls: %d"), DrawCalls);
    DrawCallTextBlock->SetText(FText::FromString(DrawCallStr));

    // 4. 更新帧率图表
    if (FpsChart.IsValid())
    {
        FpsChart->AddDataPoint(DeltaTime);
    }
}

FSlateColor SPerformancePanel::GetFpsColor(float Fps) const
{
    if (Fps >= 60.0f) return FLinearColor::Green;
    if (Fps >= 30.0f) return FLinearColor::Yellow;
    return FLinearColor::Red;
}

FSlateColor SPerformancePanel::GetMemoryColor(float UsedGB, float TotalGB) const
{
    float Ratio = (TotalGB > 0.0f) ? UsedGB / TotalGB : 0.0f;
    if (Ratio > 0.8f) return FLinearColor::Red;
    if (Ratio > 0.6f) return FLinearColor::Yellow;
    return FLinearColor::Green;
}

FString SPerformancePanel::FormatMemorySize(uint64 Bytes) const
{
    if (Bytes > 1024 * 1024 * 1024)
        return FString::Printf(TEXT("%.2f GB"), Bytes / (1024.0 * 1024.0 * 1024.0));
    if (Bytes > 1024 * 1024)
        return FString::Printf(TEXT("%.2f MB"), Bytes / (1024.0 * 1024.0));
    return FString::Printf(TEXT("%.2f KB"), Bytes / 1024.0);
}