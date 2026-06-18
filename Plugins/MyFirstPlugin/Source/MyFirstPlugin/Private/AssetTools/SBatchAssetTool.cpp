#include "SBatchAssetTool.h"
#include "ScopedTransaction.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/ScopedSlowTask.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "FileHelpers.h"
#include "Engine/Texture2D.h"

void SBatchAssetTool::Construct(const FArguments& InArgs)
{
    // 初始化压缩格式选项
    CompressionOptions.Add(MakeShareable(new FString(TEXT("BC1 (DXT1)"))));
    CompressionOptions.Add(MakeShareable(new FString(TEXT("BC3 (DXT5)"))));
    CompressionOptions.Add(MakeShareable(new FString(TEXT("BC5 (Normal Map)"))));
    CompressionOptions.Add(MakeShareable(new FString(TEXT("BC7"))));

    ChildSlot
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SVerticalBox)

                        // ===== 标题 =====
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Asset Batch Processing Tool"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                        ]

                        // ===== 操作类型选择 =====
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Operation:"))
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Copy"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnOperationChanged(EBatchOperation::Copy);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Rename"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnOperationChanged(EBatchOperation::Rename);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Compress"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnOperationChanged(EBatchOperation::CompressTextures);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(10, 0, 0, 0)
                                [
                                    SNew(STextBlock)
                                        .Text_Lambda([this]() { return GetOperationText(); })
                                ]
                        ]

                    // ===== 重命名策略选择（仅在重命名模式下显示） =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return CurrentOperation == EBatchOperation::Rename
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Strategy:"))
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Prefix"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnStrategyChanged(ERenameStrategy::AddPrefix);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Suffix"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnStrategyChanged(ERenameStrategy::AddSuffix);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Sequential"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                OnStrategyChanged(ERenameStrategy::SequentialNumber);
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(10, 0, 0, 0)
                                [
                                    SNew(STextBlock)
                                        .Text_Lambda([this]() { return GetStrategyText(); })
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Export Report"))
                                        .OnClicked(this, &SBatchAssetTool::OnExportReportClicked)
                                ]
                        ]

                    // ===== 重命名参数输入（仅在重命名模式下显示） =====
                    // 前缀输入
                    +SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return (CurrentOperation == EBatchOperation::Rename &&
                                            CurrentStrategy == ERenameStrategy::AddPrefix)
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Prefix:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(PrefixInput, SEditableTextBox)
                                        .HintText(FText::FromString("T_"))
                                ]
                        ]

                    // 后缀输入
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return (CurrentOperation == EBatchOperation::Rename &&
                                            CurrentStrategy == ERenameStrategy::AddSuffix)
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Suffix:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(SuffixInput, SEditableTextBox)
                                        .HintText(FText::FromString("_Diffuse"))
                                ]
                        ]

                    // 序号参数
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return (CurrentOperation == EBatchOperation::Rename &&
                                            CurrentStrategy == ERenameStrategy::SequentialNumber)
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Start Number:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(0.3f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(StartNumberInput, SSpinBox<int32>)
                                        .MinValue(0)
                                        .MaxValue(9999)
                                        .Value(1)
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(10, 0, 0, 0)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Digits:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(0.3f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(NumberDigitsInput, SSpinBox<int32>)
                                        .MinValue(1)
                                        .MaxValue(6)
                                        .Value(3)
                                ]
                        ]

                    // ===== 纹理压缩设置区域（仅在纹理压缩模式下显示） =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SVerticalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return CurrentOperation == EBatchOperation::CompressTextures
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(STextBlock)
                                        .Text(FText::FromString("Texture Compression Settings"))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                                ]
                                // 压缩格式下拉
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0, 5)
                                [
                                    SNew(SHorizontalBox)
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .VAlign(VAlign_Center)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("Compression:"))
                                        ]
                                        + SHorizontalBox::Slot()
                                        .FillWidth(1.0f)
                                        .Padding(5, 0, 0, 0)
                                        [
                                            SAssignNew(CompressionComboBox, SComboBox<TSharedPtr<FString>>)
                                                .OptionsSource(&CompressionOptions)
                                                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                                                    {
                                                        return SNew(STextBlock).Text(FText::FromString(*Item));
                                                    })
                                                .OnSelectionChanged(this, &SBatchAssetTool::OnCompressionSelectionChanged)
                                                [
                                                    SNew(STextBlock)
                                                        .Text_Lambda([this]() { return GetCompressionText(); })
                                                ]
                                        ]
                                ]
                            // Mip 生成下拉
                            + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0, 5)
                                [
                                    SNew(SHorizontalBox)
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .VAlign(VAlign_Center)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("Mip Gen:"))
                                        ]
                                        + SHorizontalBox::Slot()
                                        .FillWidth(1.0f)
                                        .Padding(5, 0, 0, 0)
                                        [
                                            SAssignNew(MipGenComboBox, SComboBox<TSharedPtr<FString>>)
                                                .OptionsSource(&CompressionOptions)
                                                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                                                    {
                                                        return SNew(STextBlock).Text(FText::FromString(*Item));
                                                    })
                                                .OnSelectionChanged(this, &SBatchAssetTool::OnMipGenSelectionChanged)
                                                [
                                                    SNew(STextBlock)
                                                        .Text_Lambda([this]() { return GetMipGenText(); })
                                                ]
                                        ]
                                ]
                            // SRGB 开关
                            + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0, 5)
                                [
                                    SNew(SHorizontalBox)
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .VAlign(VAlign_Center)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("sRGB:"))
                                        ]
                                        + SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .Padding(5, 0, 0, 0)
                                        [
                                            SAssignNew(SRGBCheckBox, SCheckBox)
                                                .IsChecked(ECheckBoxState::Checked)
                                        ]
                                ]
                        ]

                    // ===== 源目录 =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Source Path:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(SourcePathInput, SEditableTextBox)
                                        .HintText(FText::FromString("/Game/SourceFolder"))
                                ]
                        ]

                    // ===== 目标目录（复制模式） =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                .Visibility_Lambda([this]()
                                    {
                                        return CurrentOperation == EBatchOperation::Copy
                                            ? EVisibility::Visible : EVisibility::Collapsed;
                                    })
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Target Path:"))
                                ]
                                + SHorizontalBox::Slot()
                                .FillWidth(1.0f)
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(TargetPathInput, SEditableTextBox)
                                        .HintText(FText::FromString("/Game/TargetFolder"))
                                ]
                        ]

                    // ===== 资产类型筛选 =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                [
                                    SNew(STextBlock).Text(FText::FromString("Asset Types:"))
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(CheckTextures, SCheckBox)
                                        .IsChecked(ECheckBoxState::Checked)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("Textures"))
                                        ]
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(CheckMaterials, SCheckBox)
                                        .IsChecked(ECheckBoxState::Checked)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("Materials"))
                                        ]
                                ]
                            + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SAssignNew(CheckStaticMeshes, SCheckBox)
                                        .IsChecked(ECheckBoxState::Checked)
                                        [
                                            SNew(STextBlock).Text(FText::FromString("Static Meshes"))
                                        ]
                                ]
                        ]

                    // ===== 预览区域 =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Preview:"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SBox)
                                .MaxDesiredHeight(150)
                                [
                                    SAssignNew(PreviewList, SListView<TSharedPtr<FAssetPreviewItem>>)
                                        .ListItemsSource(&PreviewItems)
                                        .OnGenerateRow_Lambda([](TSharedPtr<FAssetPreviewItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
                                            {
                                                return SNew(STableRow<TSharedPtr<FAssetPreviewItem>>, OwnerTable)
                                                    [
                                                        SNew(SHorizontalBox)
                                                            + SHorizontalBox::Slot()
                                                            .FillWidth(0.45f)
                                                            [
                                                                SNew(STextBlock).Text(FText::FromString(Item->SourceName))
                                                            ]
                                                            + SHorizontalBox::Slot()
                                                            .AutoWidth()
                                                            [
                                                                SNew(STextBlock).Text(FText::FromString(" → "))
                                                            ]
                                                            + SHorizontalBox::Slot()
                                                            .FillWidth(0.45f)
                                                            [
                                                                SNew(STextBlock).Text(FText::FromString(Item->TargetName))
                                                            ]
                                                    ];
                                            })
                                ]
                        ]
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SAssignNew(PreviewCountText, STextBlock)
                                .Text(FText::FromString("0 assets will be affected"))
                        ]

                        // ===== 进度条 =====
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SAssignNew(ProgressBar, SProgressBar)
                                .Percent(0.0f)
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SAssignNew(ProgressText, STextBlock)
                                .Text(FText::FromString("Ready"))
                        ]

                        // ===== 按钮区域 =====
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Preview"))
                                        .OnClicked(this, &SBatchAssetTool::OnPreviewClicked)
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Execute"))
                                        .OnClicked(this, &SBatchAssetTool::OnExecuteClicked)
                                        .IsEnabled_Lambda([this]() { return !bIsExecuting && PreviewItems.Num() > 0; })
                                ]
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Cancel"))
                                        .OnClicked(this, &SBatchAssetTool::OnCancelClicked)
                                ]
                        ]
                ]
        ];
}

// ==================== 模式切换 ====================

void SBatchAssetTool::OnOperationChanged(EBatchOperation NewOperation)
{
    CurrentOperation = NewOperation;
    if (NewOperation == EBatchOperation::CompressTextures)
    {
        if (CheckTextures.IsValid()) CheckTextures->SetIsChecked(ECheckBoxState::Checked);
        if (CheckMaterials.IsValid()) CheckMaterials->SetIsChecked(ECheckBoxState::Unchecked);
        if (CheckStaticMeshes.IsValid()) CheckStaticMeshes->SetIsChecked(ECheckBoxState::Unchecked);
    }
    RefreshPreview();
}

void SBatchAssetTool::OnStrategyChanged(ERenameStrategy NewStrategy)
{
    CurrentStrategy = NewStrategy;
    RefreshPreview();
}

FText SBatchAssetTool::GetOperationText() const
{
    switch (CurrentOperation)
    {
    case EBatchOperation::Copy: return FText::FromString("Current: Copy");
    case EBatchOperation::Rename: return FText::FromString("Current: Rename");
    case EBatchOperation::CompressTextures: return FText::FromString("Current: Compress");
    default: return FText::FromString("Unknown");
    }
}

FText SBatchAssetTool::GetStrategyText() const
{
    switch (CurrentStrategy)
    {
    case ERenameStrategy::AddPrefix: return FText::FromString("Current: Prefix");
    case ERenameStrategy::AddSuffix: return FText::FromString("Current: Suffix");
    case ERenameStrategy::SequentialNumber: return FText::FromString("Current: Sequential");
    default: return FText::FromString("Unknown");
    }
}

FText SBatchAssetTool::GetCompressionText() const
{
    if (SelectedCompressionIndex >= 0 && SelectedCompressionIndex < CompressionOptions.Num())
        return FText::FromString(*CompressionOptions[SelectedCompressionIndex]);
    return FText::FromString("Select...");
}

FText SBatchAssetTool::GetMipGenText() const
{
    if (SelectedMipGenIndex >= 0 && SelectedMipGenIndex < CompressionOptions.Num())
        return FText::FromString(*CompressionOptions[SelectedMipGenIndex]);
    return FText::FromString("Select...");
}

// ==================== 压缩选择回调 ====================

void SBatchAssetTool::OnCompressionSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        for (int32 i = 0; i < CompressionOptions.Num(); i++)
            if (CompressionOptions[i] == NewSelection)
            {
                SelectedCompressionIndex = i;
                break;
            }
    }
}

void SBatchAssetTool::OnMipGenSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        for (int32 i = 0; i < CompressionOptions.Num(); i++)
            if (CompressionOptions[i] == NewSelection)
            {
                SelectedMipGenIndex = i;
                break;
            }
    }
}

// ==================== 生成目标名称 ====================

FString SBatchAssetTool::GenerateTargetName(const FString& SourceName, int32 Index) const
{
    switch (CurrentOperation)
    {
    case EBatchOperation::Copy:
        return SourceName + TEXT("_Copy");
    case EBatchOperation::Rename:
        switch (CurrentStrategy)
        {
        case ERenameStrategy::AddPrefix:
            return (PrefixInput.IsValid() ? PrefixInput->GetText().ToString() : TEXT("T_")) + SourceName;
        case ERenameStrategy::AddSuffix:
            return SourceName + (SuffixInput.IsValid() ? SuffixInput->GetText().ToString() : TEXT("_D"));
        case ERenameStrategy::SequentialNumber:
        {
            int32 StartNum = StartNumberInput.IsValid() ? StartNumberInput->GetValue() : 1;
            int32 Digits = NumberDigitsInput.IsValid() ? NumberDigitsInput->GetValue() : 3;
            return FString::Printf(TEXT("%0*d"), Digits, StartNum + Index);
        }
        default: return SourceName;
        }
    case EBatchOperation::CompressTextures:
        return SourceName;
    default:
        return SourceName;
    }
}

// ==================== 回调函数 ====================

FReply SBatchAssetTool::OnPreviewClicked()
{
    RefreshPreview();
    return FReply::Handled();
}

FReply SBatchAssetTool::OnExecuteClicked()
{
    if (bIsExecuting || PreviewItems.Num() == 0) return FReply::Handled();

    PendingOperation = CurrentOperation;
    AsyncItems = PreviewItems;
    StartAsyncExecution();

    return FReply::Handled();
}

FReply SBatchAssetTool::OnCancelClicked()
{
    if (bIsAsyncExecuting)
    {
        FTSTicker::GetCoreTicker().RemoveTicker(AsyncTickerHandle);
        bIsAsyncExecuting = false;
        ProgressText->SetText(FText::FromString("Cancelled"));
    }
    return FReply::Handled();
}

// ==================== 异步执行 ====================

void SBatchAssetTool::StartAsyncExecution()
{
    bIsAsyncExecuting = true;
    CurrentAsyncIndex = 0;
    SuccessCount = SkipCount = FailCount = 0;

    ProgressBar->SetPercent(0.0f);
    ProgressText->SetText(FText::FromString("Starting..."));

    AsyncTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool
            {
                return ProcessNextAsset(DeltaTime);
            }), 0.0f);
}

bool SBatchAssetTool::ProcessNextAsset(float DeltaTime)
{
    if (CurrentAsyncIndex >= AsyncItems.Num())
    {
        FinishAsyncExecution();
        return false;
    }

    const TSharedPtr<FAssetPreviewItem>& Item = AsyncItems[CurrentAsyncIndex];
    bool bSuccess = ProcessSingleAsset(Item, PendingOperation);
    if (bSuccess) SuccessCount++;
    else FailCount++;

    CurrentAsyncIndex++;

    float Progress = (float)CurrentAsyncIndex / (float)AsyncItems.Num();
    ProgressBar->SetPercent(Progress);
    ProgressText->SetText(FText::FromString(
        FString::Printf(TEXT("Processing: %d/%d"), CurrentAsyncIndex, AsyncItems.Num())));

    return true;
}

void SBatchAssetTool::FinishAsyncExecution()
{
    bIsAsyncExecuting = false;

    FString Report = FString::Printf(
        TEXT("Batch operation completed. Success: %d, Failed: %d, Skipped: %d"),
        SuccessCount, FailCount, SkipCount);
    ProgressText->SetText(FText::FromString(Report));
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Report);
}

// ==================== 单个资产处理（带撤销） ====================

bool SBatchAssetTool::ProcessSingleAsset(const TSharedPtr<FAssetPreviewItem>& Item, EBatchOperation Operation)
{
    if (!Item.IsValid())
        return false;

    UObject* Asset = Item->AssetData.GetAsset();
    if (!Asset)
    {
        LogOperationResult(Item->SourceName, TEXT("Load"), false, TEXT("Failed to load asset"));
        return false;
    }

    // 开启撤销事务
    FScopedTransaction Transaction(FText::FromString("Batch Asset Operation"));

    bool bSuccess = false;
    FString SourcePath = Item->AssetData.PackagePath.ToString();
    FString TargetName = Item->TargetName;
    FString OpName;

    switch (Operation)
    {
    case EBatchOperation::Copy:
    {
        OpName = TEXT("Copy");

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        IAssetTools& AssetTools = AssetToolsModule.Get();
        FString TargetPath = TargetPathInput.IsValid() ? TargetPathInput->GetText().ToString() : TEXT("/Game/Copied");
        if (TargetPath.IsEmpty()) TargetPath = TEXT("/Game/Copied");

        FString FullPath = TargetPath / TargetName;
        if (StaticLoadObject(UObject::StaticClass(), nullptr, *FullPath))
        {
            LogOperationResult(Item->SourceName, OpName, false, TEXT("Asset with same name already exists"));
            SkipCount++;
            // 记录失败历史
            OperationHistory.AddEntry(OpName, Item->SourceName, SourcePath, TargetName, false);
            return false;
        }

        Asset->Modify();
        AssetTools.DuplicateAsset(TargetName, TargetPath, Asset);
        LogOperationResult(Item->SourceName, OpName, true);
        bSuccess = true;
        break;
    }
    case EBatchOperation::Rename:
    {
        OpName = TEXT("Rename");

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        IAssetTools& AssetTools = AssetToolsModule.Get();

        TArray<FAssetRenameData> RenameArray;
        FAssetRenameData RenameData(Asset, Asset->GetOutermost()->GetName(), TargetName);
        RenameArray.Add(RenameData);

        Asset->Modify();
        if (AssetTools.RenameAssets(RenameArray))
        {
            LogOperationResult(Item->SourceName, OpName, true);
            bSuccess = true;
        }
        else
        {
            LogOperationResult(Item->SourceName, OpName, false, TEXT("RenameAssets failed"));
            bSuccess = false;
        }
        break;
    }
    case EBatchOperation::CompressTextures:
    {
        OpName = TEXT("Compress");

        UTexture2D* Texture = Cast<UTexture2D>(Asset);
        if (!Texture)
        {
            LogOperationResult(Item->SourceName, OpName, false, TEXT("Not a texture"));
            SkipCount++;
            OperationHistory.AddEntry(OpName, Item->SourceName, SourcePath, TargetName, false);
            return false;
        }

        Texture->Modify();

        // 压缩设置
        TextureCompressionSettings CompressionSetting;
        switch (SelectedCompressionIndex)
        {
        case 0: CompressionSetting = TC_Default; break;
        case 1: CompressionSetting = TC_Default; break;
        case 2: CompressionSetting = TC_Normalmap; break;
        case 3: CompressionSetting = TC_Default; break;
        default: CompressionSetting = TC_Default; break;
        }
        Texture->CompressionSettings = CompressionSetting;

        // Mip 设置（简化映射）
        TextureMipGenSettings MipSetting = TMGS_FromTextureGroup;
        if (SelectedMipGenIndex == 1) MipSetting = TMGS_NoMipmaps;
        else if (SelectedMipGenIndex >= 2 && SelectedMipGenIndex <= 6)
            MipSetting = static_cast<TextureMipGenSettings>(SelectedMipGenIndex + 2);
        Texture->MipGenSettings = MipSetting;

        Texture->SRGB = SRGBCheckBox.IsValid() && SRGBCheckBox->IsChecked();
        Texture->MarkPackageDirty();
        LogOperationResult(Item->SourceName, OpName, true);
        bSuccess = true;
        // 压缩操作的 TargetName 可以用来记录压缩格式等，这里简单保持 Item->TargetName（通常为原名称）
        break;
    }
    default:
        return false;
    }

    // 记录操作历史
    OperationHistory.AddEntry(OpName, Item->SourceName, SourcePath, TargetName, bSuccess);

    return bSuccess;
}

// ==================== 预览逻辑 ====================

void SBatchAssetTool::RefreshPreview()
{
    PreviewItems.Empty();

    FString SourcePath = SourcePathInput.IsValid() ? SourcePathInput->GetText().ToString() : TEXT("/Game");
    if (SourcePath.IsEmpty()) SourcePath = TEXT("/Game");

    IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(FName(*SourcePath));

    TArray<FAssetData> AllAssets;
    AssetRegistry.GetAssets(Filter, AllAssets);

    int32 Index = 0;
    for (const FAssetData& Asset : AllAssets)
    {
        if (!IsAssetTypeSelected(Asset)) continue;

        TSharedPtr<FAssetPreviewItem> Item = MakeShareable(new FAssetPreviewItem);
        Item->SourceName = Asset.AssetName.ToString();
        Item->TargetName = GenerateTargetName(Item->SourceName, Index);
        Item->AssetData = Asset;
        PreviewItems.Add(Item);
        Index++;
    }

    PreviewList->RequestListRefresh();
    PreviewCountText->SetText(FText::FromString(
        FString::Printf(TEXT("%d assets will be affected"), PreviewItems.Num())));
}

bool SBatchAssetTool::IsAssetTypeSelected(const FAssetData& Asset) const
{
    UClass* AssetClass = Asset.GetClass();
    if (!AssetClass) return false;

    bool bTexture = CheckTextures.IsValid() && CheckTextures->IsChecked();
    bool bMaterial = CheckMaterials.IsValid() && CheckMaterials->IsChecked();
    bool bStaticMesh = CheckStaticMeshes.IsValid() && CheckStaticMeshes->IsChecked();

    if (bTexture && AssetClass->IsChildOf(UTexture::StaticClass())) return true;
    if (bMaterial && AssetClass->IsChildOf(UMaterial::StaticClass())) return true;
    if (bStaticMesh && AssetClass->IsChildOf(UStaticMesh::StaticClass())) return true;

    return false;
}

// ==================== 日志 ====================

void SBatchAssetTool::LogOperationResult(const FString& AssetName, const FString& Action, bool bSuccess, const FString& ErrorMsg)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[BatchTool] %s: %s succeeded."), *AssetName, *Action);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[BatchTool] %s: %s failed. %s"),
            *AssetName, *Action, ErrorMsg.IsEmpty() ? TEXT("") : *ErrorMsg);
    }
}

// ==================== 导出文件回调事件 ====================

FReply SBatchAssetTool::OnExportReportClicked()
{
    // 生成带时间戳的文件名
    FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    FString FileName = FString::Printf(TEXT("BatchReport_%s.csv"), *Timestamp);

    FString ExportedPath = OperationHistory.ExportToCSV(FileName);
    if (!ExportedPath.IsEmpty())
    {
        ProgressText->SetText(FText::FromString(
            FString::Printf(TEXT("Report exported to: Saved/BatchReports/%s"), *FileName)));
    }
    else
    {
        ProgressText->SetText(FText::FromString("Export failed!"));
    }

    return FReply::Handled();
}