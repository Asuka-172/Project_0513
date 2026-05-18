// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMyFirstPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void AddMenuExtension();
	void OpenToolWindow();
private:
	// 用于移除菜单扩展的句柄
	TSharedPtr<FExtender> MenuExtender;
};
