#include "SBatchAssetTool.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/ScopedSlowTask.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"

void SBatchAssetTool::Construct(const FArguments& InArgs)
{
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
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Browse"))
                                        .OnClicked(this, &SBatchAssetTool::OnBrowseSourceClicked)
                                ]
                        ]

                    // ===== 目标目录 =====
                    + SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SHorizontalBox)
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
                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Browse"))
                                        .OnClicked(this, &SBatchAssetTool::OnBrowseTargetClicked)
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

// ==================== 回调函数 ====================

FReply SBatchAssetTool::OnBrowseSourceClicked()
{
    // 简化处理：这里可以调用系统目录对话框
    // 在编辑器插件中通常使用 FDesktopPlatformModule::OpenDirectoryDialog
    // 为保持代码简洁，此处仅作为占位
    return FReply::Handled();
}

FReply SBatchAssetTool::OnBrowseTargetClicked()
{
    return FReply::Handled();
}

FReply SBatchAssetTool::OnPreviewClicked()
{
    RefreshPreview();
    return FReply::Handled();
}

FReply SBatchAssetTool::OnExecuteClicked()
{
    if (bIsExecuting) return FReply::Handled();
    if (PreviewItems.Num() == 0) return FReply::Handled();

    bIsExecuting = true;
    ProgressBar->SetPercent(0.0f);
    ProgressText->SetText(FText::FromString("Executing..."));

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    IAssetTools& AssetTools = AssetToolsModule.Get();

    FString TargetPath = TargetPathInput->GetText().ToString();
    if (TargetPath.IsEmpty())
    {
        TargetPath = TEXT("/Game/TargetFolder");
    }

    int32 Total = PreviewItems.Num();
    int32 Processed = 0;

    for (const TSharedPtr<FAssetPreviewItem>& Item : PreviewItems)
    {
        if (!Item.IsValid()) continue;

        UObject* AssetObject = Item->AssetData.GetAsset();
        if (!AssetObject) continue;

        FString NewName = Item->TargetName;
        AssetTools.DuplicateAsset(NewName, TargetPath, AssetObject);

        Processed++;
        float Progress = (float)Processed / Total;
        ProgressBar->SetPercent(Progress);
        ProgressText->SetText(FText::FromString(
            FString::Printf(TEXT("Processing: %d/%d"), Processed, Total)));
    }

    ProgressText->SetText(FText::FromString(
        FString::Printf(TEXT("Completed! %d assets processed."), Processed)));
    bIsExecuting = false;

    return FReply::Handled();
}

FReply SBatchAssetTool::OnCancelClicked()
{
    bIsExecuting = false;
    ProgressText->SetText(FText::FromString("Cancelled"));
    return FReply::Handled();
}

// ==================== 预览逻辑 ====================

void SBatchAssetTool::RefreshPreview()
{
    PreviewItems.Empty();

    FString SourcePath = SourcePathInput->GetText().ToString();
    if (SourcePath.IsEmpty())
    {
        SourcePath = TEXT("/Game");
    }

    IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(FName(*SourcePath));

    TArray<FAssetData> AllAssets;
    AssetRegistry.GetAssets(Filter, AllAssets);

    for (const FAssetData& Asset : AllAssets)
    {
        if (!IsAssetTypeSelected(Asset)) continue;

        TSharedPtr<FAssetPreviewItem> Item = MakeShareable(new FAssetPreviewItem);
        Item->SourceName = Asset.AssetName.ToString();
        Item->TargetName = Asset.AssetName.ToString() + TEXT("_Copy");
        Item->AssetData = Asset;
        PreviewItems.Add(Item);
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