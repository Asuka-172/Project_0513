// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Engine/PostProcessVolume.h"
#include "ILevelEditor.h"

class UMPostProcessManager;
class SPostProcessPanel;
class ILevelEditor;

class FUdpChatRoom;
class SEditableTextBox;
class STextBlock;

class SFpsChart;
class SPerformancePanel;

class FAssetInfoCollector;

class FMyFirstPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void AddMenuExtension();
	void OpenToolWindow();

private:
	void OnLevelEditorCreated(TSharedPtr<ILevelEditor> LevelEditor);
	TSharedPtr<FExtender> MenuExtender;
	TSharedPtr<SPostProcessPanel> PostProcessPanel;
	APostProcessVolume* PostProcessVolume = nullptr;
	UMPostProcessManager* PostProcessManager = nullptr;
	FDelegateHandle LevelEditorCreatedHandle;
	TSharedPtr<FUdpChatRoom> ChatRoom;
	TSharedPtr<SEditableTextBox> ChatInputBox;
	TSharedPtr<STextBlock> ChatLogText;
	// 性能图表
	TSharedPtr<SFpsChart> FpsChart;
	FTSTicker::FDelegateHandle TickerHandle;
	TSharedPtr<SPerformancePanel> PerformancePanel;
	bool Tick(float DeltaTime);
	//资源检索
	TSharedPtr<STextBlock> AssetStatsText;
	TSharedPtr<FAssetInfoCollector> AssetInfoCollector;
};
