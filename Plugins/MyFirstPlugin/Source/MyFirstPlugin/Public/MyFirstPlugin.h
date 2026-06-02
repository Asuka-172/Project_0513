// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Engine/PostProcessVolume.h"
#include "ILevelEditor.h"

class UMPostProcessManager;
class SPostProcessPanel;
class ILevelEditor;

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
};
