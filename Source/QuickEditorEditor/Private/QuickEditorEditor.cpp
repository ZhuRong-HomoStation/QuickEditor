#include "QuickEditorEditor.h"
#include "Logging.h"
#include "Customization/DummyStruct.h"
#include "Customization/QEDetailCustomize.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FQuickEditorEditor"

void FQuickEditorEditor::StartupModule()
{
	// inject detail 
	if (!UObject::StaticClass()->FindPropertyByName(TEXT("QEDummy")))
	{
		FStructProperty* DummyProperty = new FStructProperty(UObject::StaticClass()
            , TEXT("QEDummy"), EObjectFlags::RF_NoFlags);
		DummyProperty->Struct = FQEDummyStruct::StaticStruct();
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Transient;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_EditConst;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Edit;
		DummyProperty->SetMetaData(TEXT("Category"), TEXT("QuickEditor"));
		UObject::StaticClass()->AddCppProperty(DummyProperty);
	}

	// add customize
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
        TEXT("QEDummyStruct"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FQEDetailCustomize::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FQuickEditorEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("QEDummyStruct");
		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuickEditorEditor, QuickEditorEditor);