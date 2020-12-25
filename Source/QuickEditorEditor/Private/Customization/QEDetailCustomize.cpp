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

TArray<UClass*>		FQEDetailCustomize::DoneDetails;

FQEDetailCustomizePerClass::FQEDetailCustomizePerClass(UClass* InTargetClass)
	: TargetClass(InTargetClass)
{
}

TSharedRef<IDetailCustomization> FQEDetailCustomizePerClass::MakeInstance(UClass* TargetClass)
{
	return MakeShareable(new FQEDetailCustomizePerClass(TargetClass));
}

void FQEDetailCustomizePerClass::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UQuickEditorService* Service = &UQuickEditorService::Get();
	
	// collect objects 
	decltype(auto) SelectedObjects = DetailBuilder.GetSelectedObjects();
	TArray<UObject*> AllObjects;
	for (auto& Object : SelectedObjects)
	{
		UObject* GotObj = Object.Get();
		if (GotObj) AllObjects.Add(GotObj);
	}

	// invoke and build 
	QEPrivate::FDetailCmdResolver Resolver(&DetailBuilder);
	QE::SelectedObjects = MoveTemp(AllObjects);
	QE::Detail::DetailState(true);
	{
		UFunction* Func = Service->DetailCustomizationMap[TargetClass];
		
		// invoke 
		FFrame Stack(nullptr, Func, nullptr);
		Func->Invoke(nullptr, Stack, nullptr);

		// resolve 
		Resolver.ResolveCommands();
		QE::CleanCommands();
	}
	QE::Detail::DetailState(false);
	FQEDetailCustomize::DoneDetails.Add(TargetClass);
}

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
	TArray<UClass*> CallChainClass;
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
			CallChainClass.Add(LowestClass);
		}
		LowestClass = LowestClass->GetSuperClass();
	}
	
	// loop call and decode for hide/show parent
	QEPrivate::FDetailCmdResolver Resolver(DetailBuilder);
	QE::SelectedObjects = MoveTemp(AllObjects);
	QE::Detail::DetailState(true);
	for (int32 i = 0; i < CallChain.Num(); ++i)
	{
		UFunction* Func = CallChain[i];
		if (DoneDetails.Contains(CallChainClass[i]))
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
	FQEDetailCustomize::DoneDetails.Reset();
}

TSharedRef<IPropertyTypeCustomization> FQEPropertyCustomize::MakeInstance(UClass* TargetClass)
{
	return MakeShareable(new FQEPropertyCustomize(TargetClass));
}

FQEPropertyCustomize::FQEPropertyCustomize(UClass* InTargetClass)
	: TargetClass(InTargetClass)
{
}

void FQEPropertyCustomize::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
										   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FQEPropertyCustomize::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}
