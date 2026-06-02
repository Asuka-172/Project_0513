#include "SPostProcessPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SCheckBox.h"

void SPostProcessPanel::Construct(const FArguments& InArgs, UMPostProcessManager* InManager)
{
    Manager = InManager; // 可以为 nullptr

    ChildSlot
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SVerticalBox)

                        // 标题
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Post Process Panel"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                        ]

                        // ----- 灰度化滤镜 -----
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Grayscale Filter"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Blend Weight:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(GrayscaleWeightSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.0f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetGrayscaleBlendWeight(Value);
                                            })
                                ]
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Contrast:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(GrayscaleContrastSlider, SSlider)
                                        .MinValue(0.5f)
                                        .MaxValue(2.0f)
                                        .Value(1.0f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetGrayscaleContrast(Value);
                                            })
                                ]
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Brightness:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(GrayscaleBrightnessSlider, SSlider)
                                        .MinValue(-0.5f)
                                        .MaxValue(0.5f)
                                        .Value(0.0f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetGrayscaleBrightness(Value);
                                            })
                                ]
                        ]

                    // ----- 颜色反转滤镜 -----
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Invert Filter"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Invert Weight:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(InvertWeightSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.0f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetInvertWeight(Value);
                                            })
                                ]
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Threshold:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(InvertThresholdSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.5f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetInvertThreshold(Value);
                                            })
                                ]
                        ]

                    // ----- 老电影滤镜 -----
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Old Film Filter"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Sepia Strength:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(SepiaStrengthSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.3f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetSepiaStrength(Value);
                                            })
                                ]
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Noise Strength:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(NoiseStrengthSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.1f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetNoiseStrength(Value);
                                            })
                                ]
                        ]

                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Vignette Strength:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                [
                                    SAssignNew(VignetteStrengthSlider, SSlider)
                                        .MinValue(0.0f)
                                        .MaxValue(1.0f)
                                        .Value(0.3f)
                                        .OnValueChanged_Lambda([this](float Value)
                                            {
                                                if (Manager) Manager->SetVignetteStrength(Value);
                                            })
                                ]
                        ]
                ]
        ];
}