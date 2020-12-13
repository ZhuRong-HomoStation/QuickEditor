#include "QuickEditorEditor.h"
#include "Logging.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FQuickEditorEditor"

void FQuickEditorEditor::StartupModule()
{
}

void FQuickEditorEditor::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuickEditorEditor, QuickEditorEditor);