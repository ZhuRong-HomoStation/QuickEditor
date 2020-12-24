#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FQuickEditorEditor : public IModuleInterface
{
public:
	// ~Begin IModuleInterface API 
	void StartupModule() override;
	void ShutdownModule() override;
	// ~End IModuleInterface API 
};
