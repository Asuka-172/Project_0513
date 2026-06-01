#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "MPostProcessManager.h"

class SPostProcessPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPostProcessPanel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UMPostProcessManager* InManager);

private:
    UMPostProcessManager* Manager = nullptr;

    // 灰度化滑块
    TSharedPtr<SSlider> GrayscaleWeightSlider;
    TSharedPtr<SSlider> GrayscaleContrastSlider;
    TSharedPtr<SSlider> GrayscaleBrightnessSlider;

    // 反转滑块
    TSharedPtr<SSlider> InvertWeightSlider;
    TSharedPtr<SSlider> InvertThresholdSlider;

    // 老电影滑块
    TSharedPtr<SSlider> SepiaStrengthSlider;
    TSharedPtr<SSlider> NoiseStrengthSlider;
    TSharedPtr<SSlider> VignetteStrengthSlider;

    // 回调
    void OnGrayscaleWeightChanged(float Value);
    void OnGrayscaleContrastChanged(float Value);
    void OnGrayscaleBrightnessChanged(float Value);
    void OnInvertWeightChanged(float Value);
    void OnInvertThresholdChanged(float Value);
    void OnSepiaStrengthChanged(float Value);
    void OnNoiseStrengthChanged(float Value);
    void OnVignetteStrengthChanged(float Value);
};