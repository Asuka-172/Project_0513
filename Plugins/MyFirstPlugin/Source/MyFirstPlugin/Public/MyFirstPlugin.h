// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Engine/PostProcessVolume.h"

class UMPostProcessManager;
class SPostProcessPanel;

class FMyFirstPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void AddMenuExtension();
	void OpenToolWindow();
private:
	TSharedPtr<FExtender> MenuExtender;
	TSharedPtr<SPostProcessPanel> PostProcessPanel;
	APostProcessVolume* PostProcessVolume = nullptr;
	UMPostProcessManager* PostProcessManager = nullptr;
};
