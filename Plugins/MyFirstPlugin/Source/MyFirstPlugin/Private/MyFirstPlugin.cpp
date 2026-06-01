// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyFirstPlugin.h"
#include "LevelEditor.h"
#include "SColorWheel.h"                    // 颜色控件
#include "SPostProcessPanel.h"              // 后处理 UI 面板
#include "MPostProcessManager.h"            // 后处理管理器
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Styling/AppStyle.h"              // 编辑器样式
#include "Framework/Application/SlateApplication.h"
#include "Engine/PostProcessVolume.h"       // APostProcessVolume
#include "EngineUtils.h"                    // TActorIterator
#include "Editor.h"                         // GEditor

#define LOCTEXT_NAMESPACE "FMyFirstPluginModule"

// 获取或创建全局的 PostProcessVolume（无限范围）
static APostProcessVolume* FindOrCreatePostProcessVolume()
{
    // 获取当前编辑器世界
    UWorld* World = nullptr;
    if (GEditor)
    {
        World = GEditor->GetEditorWorldContext().World();
    }
    if (!World) return nullptr;

    // 先查找关卡中是否已有无限范围的 PostProcessVolume
    for (TActorIterator<APostProcessVolume> It(World); It; ++It)
    {
        if (It->bUnbound)
        {
            return *It;
        }
    }

    // 没有就创建一个
    FActorSpawnParameters SpawnParams;
    APostProcessVolume* NewVolume = World->SpawnActor<APostProcessVolume>(SpawnParams);
    if (NewVolume)
    {
        NewVolume->bUnbound = true;          // 对整个关卡生效
        NewVolume->SetActorLabel(TEXT("MyPlugin_PPV")); // 方便在编辑器中识别
    }
    return NewVolume;
}

void FMyFirstPluginModule::AddMenuExtension()
{
    FLevelEditorModule& LevelEditorModule =
        FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);

    Extender->AddMenuExtension(
        "WindowLayout",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
            {
                MenuBuilder.AddMenuEntry(
                    FText::FromString("My First Tool"),
                    FText::FromString("Open My First Tool Window"),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenToolWindow))
                );
            })
    );

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
    MenuExtender = Extender;
}

void FMyFirstPluginModule::OpenToolWindow()
{
    // ==================== 初始化后处理系统（仅首次） ====================
    if (!PostProcessManager)
    {
        PostProcessVolume = FindOrCreatePostProcessVolume();
        if (PostProcessVolume)
        {
            PostProcessManager = NewObject<UMPostProcessManager>();
            PostProcessManager->Initialize(PostProcessVolume);
        }
    }

    // 创建后处理 UI 面板（每次打开窗口都重建，以便实时反映最新参数）
    TSharedPtr<SPostProcessPanel> LocalPanel;
    if (PostProcessManager)
    {
        LocalPanel = SNew(SPostProcessPanel, PostProcessManager);
    }
    PostProcessPanel = LocalPanel; // 保存到成员变量

    // ==================== 构建窗口内容 ====================
    TSharedPtr<STextBlock> ColorText;

    TSharedRef<SWidget> WindowContent = SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SNew(SVerticalBox)

                // ---------- 颜色选择器区域 ----------
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Color Picker"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SAssignNew(ColorText, STextBlock)
                        .Text(FText::FromString("RGB: (1.00, 0.00, 0.00)"))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SColorWheel)
                        .OnColorChanged(FOnColorChanged::CreateLambda(
                            [ColorText](FLinearColor NewColor)
                            {
                                if (ColorText.IsValid())
                                {
                                    FString RGBStr = FString::Printf(
                                        TEXT("RGB: (%.2f, %.2f, %.2f)"),
                                        NewColor.R, NewColor.G, NewColor.B);
                                    ColorText->SetText(FText::FromString(RGBStr));
                                }
                            }))
                ]

            // ---------- 后处理滤镜区域 ----------
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Post Process Filters"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]

                // 嵌入后处理面板
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    LocalPanel.IsValid()
                        ? TSharedRef<SWidget>(LocalPanel.ToSharedRef())
                        : SNew(STextBlock).Text(FText::FromString("PostProcessManager not ready"))
                ]

                // ---------- 调试按钮 ----------
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Print to Log"))
                        .OnClicked_Lambda([]() -> FReply
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Button clicked"));
                                return FReply::Handled();
                            })
                ]
        ]; // end SVerticalBox

    TSharedRef<SWindow> ToolWindow = SNew(SWindow)
        .Title(FText::FromString("My First Tool"))
        .ClientSize(FVector2D(600, 600))
        [
            WindowContent
        ];

    FSlateApplication::Get().AddWindow(ToolWindow);
}

void FMyFirstPluginModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module started!"));
    AddMenuExtension();
}

void FMyFirstPluginModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module shutting down."));

    // 移除菜单扩展
    if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
    {
        FLevelEditorModule& LevelEditorModule =
            FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        if (MenuExtender.IsValid())
        {
            LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
        }
    }

    // 清理后处理效果（移除 Blendables）
    if (PostProcessManager)
    {
        PostProcessManager->Shutdown();
        PostProcessManager = nullptr;
    }

    // 注意：我们不销毁 PostProcessVolume，只把它还原成空白状态
    PostProcessVolume = nullptr;
    PostProcessPanel.Reset();
}

#undef LOCTEXT_NAMESPACE