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
#include "Network/FUdpChatRoom.h"
#include "SFpsChart/SFpsChart.h"
#include "SFpsChart/SPerformancePanel.h"
#include "AssetTools/FAssetInfoCollector.h"
#include "AssetTools/FMyAssetActions.h"


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

    // 注册定时器，每帧更新FPS图表
    TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateRaw(this, &FMyFirstPluginModule::Tick));

    // 注册资产右键菜单
    IAssetTools& AssetTools =
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    TSharedRef<FMyAssetActions> TextureActions = MakeShareable(new FMyAssetActions(UTexture::StaticClass()));
    AssetTools.RegisterAssetTypeActions(TextureActions);          // 注册
    RegisteredAssetActions.Add(TextureActions);                   // 保存共享指针以便卸载

    TSharedRef<FMyAssetActions> MaterialActions = MakeShareable(new FMyAssetActions(UMaterial::StaticClass()));
    AssetTools.RegisterAssetTypeActions(MaterialActions);
    RegisteredAssetActions.Add(MaterialActions);
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

    //移除定时器
    if (TickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
    }
    PerformancePanel.Reset();
    FpsChart.Reset();

    // 取消注册资产 Actions
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools =
            FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (TSharedPtr<FAssetTypeActions_Base>& Action : RegisteredAssetActions)
        {
            if (Action.IsValid())
            {
                AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
            }
        }
    }
    RegisteredAssetActions.Empty();

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
                                            if (Client.Connect(TEXT("127.0.0.1"), 12346))
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

            // ----- UDP 聊天区域 -----
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("UDP Chat Room"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]

                // 聊天消息显示区
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SAssignNew(ChatLogText, STextBlock)
                        .Text(FText::FromString("Chat log will appear here..."))
                        .AutoWrapText(true)
                ]

                // 输入框 + 发送按钮
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SAssignNew(ChatInputBox, SEditableTextBox)
                                .HintText(FText::FromString("Type a message..."))
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(5, 0, 0, 0)
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Send"))
                                .OnClicked_Lambda([this]() -> FReply
                                    {
                                        if (ChatRoom.IsValid() && ChatInputBox.IsValid())
                                        {
                                            FString Message = ChatInputBox->GetText().ToString();
                                            if (!Message.IsEmpty())
                                            {
                                                // 发送给自己（本地回环），演示用
                                                ChatRoom->SendMessage(Message, TEXT("127.0.0.1"), 12400);
                                                ChatInputBox->SetText(FText::GetEmpty());
                                            }
                                        }
                                        return FReply::Handled();
                                    })
                        ]
                ]

            // 启动聊天室按钮
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Start Chat Room"))
                        .OnClicked_Lambda([this]() -> FReply
                            {
                                static bool bChatStarted = false;
                                if (!bChatStarted)
                                {
                                    ChatRoom = MakeShareable(new FUdpChatRoom);
                                    if (ChatRoom->Initialize(12400))  // 使用端口12346
                                    {
                                        bChatStarted = true;
                                        // 设置定时器轮询消息队列（因Slate窗口不在Tick中，我们用Timer或手动刷新）
                                    }
                                }
                                return FReply::Handled();
                            })
                ]

            // 刷新按钮（手动拉取新消息）
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(SButton)
                        .Text(FText::FromString("Refresh"))
                        .OnClicked_Lambda([this]() -> FReply
                            {
                                if (ChatRoom.IsValid() && ChatLogText.IsValid())
                                {
                                    FString AllMessages;
                                    FString Msg, Sender;
                                    while (ChatRoom->GetNextMessage(Msg, Sender))
                                    {
                                        AllMessages += FString::Printf(TEXT("[%s]: %s\n"), *Sender, *Msg);
                                    }
                                    if (!AllMessages.IsEmpty())
                                    {
                                        ChatLogText->SetText(FText::FromString(AllMessages));
                                    }
                                }
                                return FReply::Handled();
                            })
                ]

            // ----- 性能监控面板 -----
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Performance Monitor"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SAssignNew(PerformancePanel, SPerformancePanel)
                ]

            // ----- 资源检索 区域 -----
            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Asset Statistics"))
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
                                .Text(FText::FromString("Refresh Stats"))
                                .OnClicked_Lambda([this]() -> FReply
                                    {
                                        if (!AssetInfoCollector.IsValid())
                                        {
                                            AssetInfoCollector = MakeShareable(new FAssetInfoCollector);
                                        }
                                        AssetInfoCollector->Refresh();

                                        if (AssetStatsText.IsValid())
                                        {
                                            FString Stats = FString::Printf(
                                                TEXT("Total Assets: %d\n")
                                                TEXT("Textures: %d\n")
                                                TEXT("Materials: %d\n")
                                                TEXT("Blueprints: %d\n")
                                                TEXT("Static Meshes: %d"),
                                                AssetInfoCollector->GetTotalCount(),
                                                AssetInfoCollector->GetTextureCount(),
                                                AssetInfoCollector->GetMaterialCount(),
                                                AssetInfoCollector->GetBlueprintCount(),
                                                AssetInfoCollector->GetStaticMeshCount()
                                            );
                                            AssetStatsText->SetText(FText::FromString(Stats));
                                        }
                                        return FReply::Handled();
                                    })
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10)
                [
                    SAssignNew(AssetStatsText, STextBlock)
                        .Text(FText::FromString("Click Refresh to load statistics..."))
                        .AutoWrapText(true)
                ]

        ];

    TSharedRef<SWindow> ToolWindow = SNew(SWindow)
        .Title(FText::FromString("My First Tool"))
        .ClientSize(FVector2D(600, 600))
        [WindowContent];

    FSlateApplication::Get().AddWindow(ToolWindow);
}

//实现Tick函数
bool FMyFirstPluginModule::Tick(float DeltaTime)
{
    if (PerformancePanel.IsValid())
    {
        PerformancePanel->UpdateStats(DeltaTime);
    }
    return true; // 继续执行
}

IMPLEMENT_MODULE(FMyFirstPluginModule, MyFirstPlugin)
#undef LOCTEXT_NAMESPACE