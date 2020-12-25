#include "QEDetailCustomize.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "Logging.h"
#include "PropertyCustomizationHelpers.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"
#include "Help/CommandResolvers.h"
#include "Services/QuickEditorService.h"

TSharedRef<IPropertyTypeCustomization> FQEDetailCustomize::MakeInstance()
{
	return MakeShareable(new FQEDetailCustomize());
}

void FQEDetailCustomize::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FQEDetailCustomize::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// init info 
	IDetailLayoutBuilder* DetailBuilder = &ChildBuilder.GetParentCategory().GetParentLayout();
	TArray<UObject*> AllObjects;
	PropertyHandle->GetOuterObjects(AllObjects);
	TArray<UFunction*> CallChain;
	UQuickEditorService* Service = &UQuickEditorService::Get();
	UClass* LowestClass = AllObjects[0]->GetClass();

	// hide self
	DetailBuilder->HideCategory(TEXT("QuickEditor"));

	// find lowest class
	for (int32 i = 0; i < AllObjects.Num(); ++i)
	{
		if (LowestClass->IsChildOf(AllObjects[i]->GetClass()))
		{
			LowestClass = AllObjects[i]->GetClass();
		}
	}
	
	// search detail call chain
	while (LowestClass)
	{
		UFunction** Func = Service->DetailCustomizationMap.Find(LowestClass);
		if (Func)
		{
			CallChain.Add(*Func);
		}
		LowestClass = LowestClass->GetSuperClass();
	}
	
	// loop call and decode for hide/show parent
	QEPrivate::FDetailCmdResolver Resolver(DetailBuilder, &CustomizationUtils);
	QE::SelectedObjects = MoveTemp(AllObjects);
	QE::Detail::DetailState(true);
	for (UFunction* Func : CallChain)
	{
		if (!Resolver.QueryShowState(Func->GetOuter()->GetClass()->GetName()))
		{
			continue;
		}

		// invoke 
		FFrame Stack(nullptr, Func, nullptr);
		Func->Invoke(nullptr, Stack, nullptr);

		// resolve 
		Resolver.ResolveCommands();
		QE::CleanCommands();
	}
	QE::Detail::DetailState(false);
}

TSharedRef<IPropertyTypeCustomization> FQEPropertyCustomize::MakeInstance()
{
	return MakeShareable(new FQEPropertyCustomize());
}

void FQEPropertyCustomize::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
										   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FQEPropertyCustomize::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
