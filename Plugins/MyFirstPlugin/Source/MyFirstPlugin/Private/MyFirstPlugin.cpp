#include "MyFirstPlugin.h"
#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "SColorWheel.h"
#include "SPostProcessPanel.h"
#include "MPostProcessManager.h"
#include "Engine/PostProcessVolume.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Network/FTcpEchoServer.h"
#include "Network/FTcpEchoClient.h"

#define LOCTEXT_NAMESPACE "FMyFirstPluginModule"

// 辅助函数：查找或创建无限范围的 PostProcessVolume
static APostProcessVolume* FindOrCreatePostProcessVolume()
{
    UWorld* World = nullptr;
    if (GEditor)
    {
        World = GEditor->GetEditorWorldContext().World();
    }
    if (!World) return nullptr;

    for (TActorIterator<APostProcessVolume> It(World); It; ++It)
    {
        if (It->bUnbound) return *It;
    }

    FActorSpawnParameters SpawnParams;
    APostProcessVolume* NewVolume = World->SpawnActor<APostProcessVolume>(SpawnParams);
    if (NewVolume)
    {
        NewVolume->bUnbound = true;
        NewVolume->SetActorLabel(TEXT("MyPlugin_PPV"));
    }
    return NewVolume;
}

void FMyFirstPluginModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module started!"));
    AddMenuExtension();
}

void FMyFirstPluginModule::ShutdownModule()
{
    // 清理后处理系统
    if (PostProcessManager)
    {
        PostProcessManager->Shutdown();
        PostProcessManager = nullptr;
    }
    PostProcessVolume = nullptr;
    PostProcessPanel.Reset();

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
}

void FMyFirstPluginModule::AddMenuExtension()
{
    FLevelEditorModule& LevelEditorModule =
        FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);
    Extender->AddMenuExtension(
        "WindowLayout", EExtensionHook::After, nullptr,
        FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
            {
                MenuBuilder.AddMenuEntry(
                    FText::FromString("My First Tool"),
                    FText::FromString("Open My First Tool Window"),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenToolWindow)));
            }));

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
    MenuExtender = Extender;
}

void FMyFirstPluginModule::OpenToolWindow()
{
    // ----- 初始化后处理系统 -----
    if (!PostProcessVolume)
    {
        PostProcessVolume = FindOrCreatePostProcessVolume();
    }
    if (!PostProcessManager)
    {
        PostProcessManager = NewObject<UMPostProcessManager>();
    }
    if (PostProcessManager && PostProcessVolume)
    {
        PostProcessManager->Initialize(PostProcessVolume);
    }

    // ----- 构建 UI -----
    TSharedPtr<STextBlock> ColorText;

    TSharedRef<SWidget> WindowContent = SNew(SScrollBox)
        + SScrollBox::Slot()
        [
            SNew(SVerticalBox)

                // 颜色选择器区域
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Color Picker"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SAssignNew(ColorText, STextBlock)
                        .Text(FText::FromString("RGB: (1.00, 0.00, 0.00)"))
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(10)
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

            // 后处理滤镜区域
            + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Post Process Filters"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SAssignNew(PostProcessPanel, SPostProcessPanel, PostProcessManager)
                ]

                // 调试按钮
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Print to Log"))
                        .OnClicked_Lambda([]() -> FReply
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Button clicked"));
                                return FReply::Handled();
                            })
                ]

            // ----- 网络测试区 -----
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("TCP Echo Test"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Start Server"))
                                .OnClicked_Lambda([]() -> FReply
                                    {
                                        static FTcpEchoServer Server; // 保持生命周期
                                        static bool bStarted = false;
                                        if (!bStarted)
                                        {
                                            if (Server.Start(12345))
                                                bStarted = true;
                                        }
                                        return FReply::Handled();
                                    })
                        ]
                    + SHorizontalBox::Slot()
                        .Padding(10, 0, 0, 0)
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Send Echo"))
                                .OnClicked_Lambda([]() -> FReply
                                    {
                                        static FTcpEchoClient Client;
                                        static bool bConnected = false;
                                        if (!bConnected)
                                        {
                                            if (Client.Connect(TEXT("127.0.0.1"), 12345))
                                            {
                                                bConnected = true;
                                                Client.SendMessage(TEXT("Hello, Server!"));
                                            }
                                        }
                                        else
                                        {
                                            Client.SendMessage(TEXT("Hello again!"));
                                        }
                                        return FReply::Handled();
                                    })
                        ]
                ]
        ];

    TSharedRef<SWindow> ToolWindow = SNew(SWindow)
        .Title(FText::FromString("My First Tool"))
        .ClientSize(FVector2D(600, 600))
        [WindowContent];

    FSlateApplication::Get().AddWindow(ToolWindow);
}

IMPLEMENT_MODULE(FMyFirstPluginModule, MyFirstPlugin)
#undef LOCTEXT_NAMESPACE