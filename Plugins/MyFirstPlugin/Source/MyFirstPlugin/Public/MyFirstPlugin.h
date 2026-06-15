// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Engine/PostProcessVolume.h"
#include "ILevelEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"

class UMPostProcessManager;
class SPostProcessPanel;
class ILevelEditor;

class FUdpChatRoom;
class SEditableTextBox;
class STextBlock;

class SFpsChart;
class SPerformancePanel;

class FAssetInfoCollector;
class FAssetTypeActions_Base;

class FMyFirstPluginModule : public IModuleInterface
{
public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    void AddMenuExtension();

    // 独立的功能窗口
    void OpenColorPickerWindow();
    void OpenPostProcessWindow();
    void OpenNetworkToolsWindow();
    void OpenPerformanceWindow();
    void OpenAssetToolsWindow();
    void OpenBatchAssetToolWindow();

private:
    TSharedPtr<FExtender> MenuExtender;

    // 后处理系统
    TSharedPtr<SPostProcessPanel> PostProcessPanel;
    APostProcessVolume* PostProcessVolume = nullptr;
    UMPostProcessManager* PostProcessManager = nullptr;

    // 网络工具
    TSharedPtr<FUdpChatRoom> ChatRoom;

    // 性能监控
    FTSTicker::FDelegateHandle TickerHandle;
    TSharedPtr<SPerformancePanel> PerformancePanel;
    TSharedPtr<SFpsChart> FpsChart;
    bool Tick(float DeltaTime);

    // 资产统计
    TSharedPtr<FAssetInfoCollector> AssetInfoCollector;

    // 右键菜单
    TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetActions;
};