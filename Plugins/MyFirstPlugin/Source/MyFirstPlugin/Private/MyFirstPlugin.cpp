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
#include "AssetTools/SBatchAssetTool.h"

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

// ==================== 模块启动与关闭 ====================

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
    AssetTools.RegisterAssetTypeActions(TextureActions);
    RegisteredAssetActions.Add(TextureActions);

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

    // 移除定时器
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

    UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module shutting down."));
}

// ==================== 菜单扩展（子菜单结构） ====================

void FMyFirstPluginModule::AddMenuExtension()
{
    FLevelEditorModule& LevelEditorModule =
        FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);
    Extender->AddMenuExtension(
        "WindowLayout", EExtensionHook::After, nullptr,
        FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
            {
                // 添加子菜单
                MenuBuilder.AddSubMenu(
                    FText::FromString("My First Tool"),
                    FText::FromString("Open a tool window"),
                    FNewMenuDelegate::CreateLambda([this](FMenuBuilder& SubMenuBuilder)
                        {
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Color Picker"),
                                FText::FromString("Open color picker and RGB display"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenColorPickerWindow))
                            );
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Post Process Filters"),
                                FText::FromString("Adjust grayscale, invert, old film effects"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenPostProcessWindow))
                            );
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Network Tools"),
                                FText::FromString("TCP Echo and UDP Chat"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenNetworkToolsWindow))
                            );
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Performance Monitor"),
                                FText::FromString("FPS chart, memory, draw calls"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenPerformanceWindow))
                            );
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Asset Statistics"),
                                FText::FromString("Refresh and view asset counts by type"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenAssetToolsWindow))
                            );
                            SubMenuBuilder.AddMenuEntry(
                                FText::FromString("Batch Asset Tool"),
                                FText::FromString("Open batch asset processing tool"),
                                FSlateIcon(),
                                FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenBatchAssetToolWindow))
                            );
                        }),
                    false
                );
            })
    );

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
    MenuExtender = Extender;
}

// ==================== 独立功能窗口 ====================

void FMyFirstPluginModule::OpenColorPickerWindow()
{
    TSharedPtr<STextBlock> ColorText;

    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Color Picker"))
        .ClientSize(FVector2D(500, 200))
        [
            SNew(SVerticalBox)
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
        ];

    FSlateApplication::Get().AddWindow(Window);
}

void FMyFirstPluginModule::OpenPostProcessWindow()
{
    // 确保后处理系统初始化
    if (!PostProcessVolume)
        PostProcessVolume = FindOrCreatePostProcessVolume();
    if (!PostProcessManager)
        PostProcessManager = NewObject<UMPostProcessManager>();
    if (PostProcessManager && PostProcessVolume)
        PostProcessManager->Initialize(PostProcessVolume);

    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Post Process Filters"))
        .ClientSize(FVector2D(500, 600))
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SAssignNew(PostProcessPanel, SPostProcessPanel, PostProcessManager)
                ]
        ];

    FSlateApplication::Get().AddWindow(Window);
}

void FMyFirstPluginModule::OpenNetworkToolsWindow()
{
    TSharedPtr<STextBlock> ChatLogText;
    TSharedPtr<SEditableTextBox> ChatInputBox;

    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Network Tools"))
        .ClientSize(FVector2D(500, 600))
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SVerticalBox)

                        // ===== TCP Echo 测试 =====
                        + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("TCP Echo Test"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                        ]
                        + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot().AutoWidth()
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Start Server"))
                                        .OnClicked_Lambda([]() -> FReply
                                            {
                                                static FTcpEchoServer Server;
                                                static bool bStarted = false;
                                                if (!bStarted)
                                                {
                                                    if (Server.Start(12345))
                                                        bStarted = true;
                                                }
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot().Padding(10, 0, 0, 0)
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

                    // ===== UDP 聊天室 =====
                    + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("UDP Chat Room"))
                                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                        ]
                        + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Chat Log:"))
                        ]
                        + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SAssignNew(ChatLogText, STextBlock)
                                .Text(FText::FromString("Chat log will appear here..."))
                                .AutoWrapText(true)
                        ]
                        + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot().FillWidth(1.0f)
                                [
                                    SAssignNew(ChatInputBox, SEditableTextBox)
                                        .HintText(FText::FromString("Type a message..."))
                                ]
                                + SHorizontalBox::Slot().AutoWidth().Padding(5, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Send"))
                                        .OnClicked_Lambda([this, ChatInputBox]() -> FReply
                                            {
                                                if (ChatRoom.IsValid() && ChatInputBox.IsValid())
                                                {
                                                    FString Message = ChatInputBox->GetText().ToString();
                                                    if (!Message.IsEmpty())
                                                    {
                                                        ChatRoom->SendMessage(Message, TEXT("127.0.0.1"), 12400);
                                                        ChatInputBox->SetText(FText::GetEmpty());
                                                    }
                                                }
                                                return FReply::Handled();
                                            })
                                ]
                        ]
                    + SVerticalBox::Slot().AutoHeight().Padding(10)
                        [
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot().AutoWidth()
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Start Chat Room"))
                                        .OnClicked_Lambda([this]() -> FReply
                                            {
                                                if (!ChatRoom.IsValid())
                                                {
                                                    ChatRoom = MakeShareable(new FUdpChatRoom);
                                                    ChatRoom->Initialize(12400);
                                                }
                                                return FReply::Handled();
                                            })
                                ]
                            + SHorizontalBox::Slot().Padding(10, 0, 0, 0)
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Refresh"))
                                        .OnClicked_Lambda([this, ChatLogText]() -> FReply
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
                        ]
                ]
        ];

    FSlateApplication::Get().AddWindow(Window);
}

void FMyFirstPluginModule::OpenPerformanceWindow()
{
    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Performance Monitor"))
        .ClientSize(FVector2D(500, 400))
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Performance Monitor"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SAssignNew(PerformancePanel, SPerformancePanel)
                ]
        ];

    FSlateApplication::Get().AddWindow(Window);
}

void FMyFirstPluginModule::OpenAssetToolsWindow()
{
    TSharedPtr<STextBlock> AssetStatsText;

    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Asset Statistics"))
        .ClientSize(FVector2D(500, 300))
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Asset Statistics"))
                        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
                ]
                + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Refresh Stats"))
                                .OnClicked_Lambda([this, AssetStatsText]() -> FReply
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
            + SVerticalBox::Slot().AutoHeight().Padding(10)
                [
                    SAssignNew(AssetStatsText, STextBlock)
                        .Text(FText::FromString("Click Refresh to load statistics..."))
                        .AutoWrapText(true)
                ]
        ];

    FSlateApplication::Get().AddWindow(Window);
}

void FMyFirstPluginModule::OpenBatchAssetToolWindow()
{
    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Batch Asset Tool"))
        .ClientSize(FVector2D(600, 700))
        [
            SNew(SBatchAssetTool)
        ];

    FSlateApplication::Get().AddWindow(Window);
}

// ==================== Tick ====================

bool FMyFirstPluginModule::Tick(float DeltaTime)
{
    if (PerformancePanel.IsValid())
    {
        PerformancePanel->UpdateStats(DeltaTime);
    }
    return true;
}

IMPLEMENT_MODULE(FMyFirstPluginModule, MyFirstPlugin)
#undef LOCTEXT_NAMESPACE