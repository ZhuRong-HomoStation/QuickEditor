#include "Modules/ModuleManager.h"

class FQuickEditorModule : public IModuleInterface
{
public:
	// ~Begin IModuleInterface API 
	virtual void StartupModule() override { }
	virtual void ShutdownModule() override { }
	// ~End IModuleInterface API 
};
	
IMPLEMENT_MODULE(FQuickEditorModule, QuickEditor)