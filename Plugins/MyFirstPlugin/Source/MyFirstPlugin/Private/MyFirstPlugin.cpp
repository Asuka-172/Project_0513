// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyFirstPlugin.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Styling/AppStyle.h"          // 用于 FEditorStyle
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "FMyFirstPluginModule"

void FMyFirstPluginModule::AddMenuExtension()
{
    // 获取 Level Editor 模块
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    // 创建一个扩展器，用于在菜单栏上添加菜单项
    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);

    // 扩展 Window 菜单（After 后面的 Hook 名称可以是 "Window" 或 "Help"，我们插在 Window 菜单之后）
    Extender->AddMenuExtension(
        "WindowLayout",                 // 参考点，位于 Window 菜单内某个项之后
        EExtensionHook::After,          // 在参考点之后插入
        nullptr,
        FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
            {
                // 添加按钮
                MenuBuilder.AddMenuEntry(
                    FText::FromString("My First Tool"),    // 菜单项名称
                    FText::FromString("Open My First Tool Window"), // 提示
                    FSlateIcon(),                           // 图标（空）
                    FUIAction(FExecuteAction::CreateRaw(this, &FMyFirstPluginModule::OpenToolWindow))
                );
            })
    );

    // 将扩展器添加到菜单栏
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
    MenuExtender = Extender; // 保存以便 Shutdown 时移除
}

void FMyFirstPluginModule::OpenToolWindow()
{
    // 创建输入框的文本变量（共享状态）
    TSharedPtr<SEditableTextBox> InputBox;

    // 创建窗口内容
    TSharedRef<SWidget> WindowContent = SNew(SVerticalBox)

        // 标题
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(STextBlock)
                .Text(FText::FromString("Welcome to My First Tool"))
                .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 16))
        ]

        // 提示文本
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(STextBlock)
                .Text(FText::FromString("Type something and click the button:"))
        ]

        // 输入框
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SAssignNew(InputBox, SEditableTextBox)
                .HintText(FText::FromString("Enter text here..."))
        ]

        // 按钮
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SButton)
                .Text(FText::FromString("Print to Log"))
                .OnClicked_Lambda([InputBox]() -> FReply
                    {
                        FString Text = InputBox->GetText().ToString();
                        UE_LOG(LogTemp, Warning, TEXT("User typed: %s"), *Text);
                        return FReply::Handled();
                    })
        ];

    // 创建窗口
    TSharedRef<SWindow> ToolWindow = SNew(SWindow)
        .Title(FText::FromString("My First Tool"))
        .ClientSize(FVector2D(500, 300))
        [
            WindowContent
        ];

    // 显示窗口
    FSlateApplication::Get().AddWindow(ToolWindow);
}

void FMyFirstPluginModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module started!"));
	AddMenuExtension();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMyFirstPluginModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("MyFirstPlugin: Module shutting down."));
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    // 移除菜单扩展，防止编辑器关闭时崩溃
    if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        if (MenuExtender.IsValid())
        {
            LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
        }
    }
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMyFirstPluginModule, MyFirstPlugin)