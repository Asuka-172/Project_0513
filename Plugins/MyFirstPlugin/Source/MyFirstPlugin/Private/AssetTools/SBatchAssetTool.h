#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "AssetRegistry/AssetData.h"

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
    // 路径输入
    TSharedPtr<SEditableTextBox> SourcePathInput;
    TSharedPtr<SEditableTextBox> TargetPathInput;

    // 资产类型筛选
    TSharedPtr<SCheckBox> CheckTextures;
    TSharedPtr<SCheckBox> CheckMaterials;
    TSharedPtr<SCheckBox> CheckStaticMeshes;

    // 预览列表
    TSharedPtr<SListView<TSharedPtr<FAssetPreviewItem>>> PreviewList;
    TArray<TSharedPtr<FAssetPreviewItem>> PreviewItems;
    TSharedPtr<STextBlock> PreviewCountText;

    // 进度条
    TSharedPtr<SProgressBar> ProgressBar;
    TSharedPtr<STextBlock> ProgressText;

    // 执行状态
    bool bIsExecuting = false;

    // 回调函数
    FReply OnBrowseSourceClicked();
    FReply OnBrowseTargetClicked();
    FReply OnPreviewClicked();
    FReply OnExecuteClicked();
    FReply OnCancelClicked();

    // 预览：扫描源目录，生成受影响的资产列表
    void RefreshPreview();

    // 判断资产类型是否被选中
    bool IsAssetTypeSelected(const FAssetData& Asset) const;
};