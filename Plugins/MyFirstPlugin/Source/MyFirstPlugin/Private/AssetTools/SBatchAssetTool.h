#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Input/SComboBox.h"
#include "AssetRegistry/AssetData.h"
#include "Engine/TextureDefines.h"

// 操作类型
enum class EBatchOperation : uint8
{
    Copy,
    Rename,
    CompressTextures
};

// 重命名策略
enum class ERenameStrategy : uint8
{
    AddPrefix,
    AddSuffix,
    SequentialNumber
};

// 预览列表项
struct FAssetPreviewItem
{
    FString SourceName;
    FString TargetName;
    FAssetData AssetData;
};

class SBatchAssetTool : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBatchAssetTool) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    // ===== 操作模式 =====
    EBatchOperation CurrentOperation = EBatchOperation::Copy;
    ERenameStrategy CurrentStrategy = ERenameStrategy::AddPrefix;

    // ===== 路径输入 =====
    TSharedPtr<SEditableTextBox> SourcePathInput;
    TSharedPtr<SEditableTextBox> TargetPathInput;

    // ===== 重命名参数 =====
    TSharedPtr<SEditableTextBox> PrefixInput;
    TSharedPtr<SEditableTextBox> SuffixInput;
    TSharedPtr<SSpinBox<int32>> StartNumberInput;
    TSharedPtr<SSpinBox<int32>> NumberDigitsInput;

    // ===== 纹理压缩参数 =====
    TArray<TSharedPtr<FString>> CompressionOptions;   // 下拉列表数据源
    TSharedPtr<SComboBox<TSharedPtr<FString>>> CompressionComboBox; // 压缩格式下拉
    TSharedPtr<SComboBox<TSharedPtr<FString>>> MipGenComboBox;      // Mip 生成下拉
    TSharedPtr<SCheckBox> SRGBCheckBox;              // SRGB 开关
    int32 SelectedCompressionIndex = 0;               // 当前选中压缩格式索引
    int32 SelectedMipGenIndex = 0;                    // 当前选中 Mip 生成索引

    // ===== 资产类型筛选 =====
    TSharedPtr<SCheckBox> CheckTextures;
    TSharedPtr<SCheckBox> CheckMaterials;
    TSharedPtr<SCheckBox> CheckStaticMeshes;

    // ===== 预览列表 =====
    TSharedPtr<SListView<TSharedPtr<FAssetPreviewItem>>> PreviewList;
    TArray<TSharedPtr<FAssetPreviewItem>> PreviewItems;
    TSharedPtr<STextBlock> PreviewCountText;

    // ===== 进度条 =====
    TSharedPtr<SProgressBar> ProgressBar;
    TSharedPtr<STextBlock> ProgressText;

    // ===== 执行状态 =====
    bool bIsExecuting = false;

    // ===== 回调 =====
    FReply OnPreviewClicked();
    FReply OnExecuteClicked();
    FReply OnCancelClicked();

    // 操作模式切换
    void OnOperationChanged(EBatchOperation NewOperation);
    void OnStrategyChanged(ERenameStrategy NewStrategy);

    // 根据当前策略生成目标名称
    FString GenerateTargetName(const FString& SourceName, int32 Index) const;

    // 预览刷新
    void RefreshPreview();

    // 执行批量复制
    void ExecuteBatchCopy();
    // 执行批量重命名
    void ExecuteBatchRename();
	// 执行批量纹理压缩
    void ExecuteTextureCompression();

    // 判断资产类型是否被选中
    bool IsAssetTypeSelected(const FAssetData& Asset) const;

    // 获取操作模式文本
    FText GetOperationText() const;
    FText GetStrategyText() const;

    // 获取当前压缩设置文本
    FText GetCompressionText() const;
    FText GetMipGenText() const;

    // 压缩选择回调
    void OnCompressionSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
    void OnMipGenSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
};