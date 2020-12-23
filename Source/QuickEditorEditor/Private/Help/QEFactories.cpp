#include "QEFactories.h"

#include "EditorReimportHandler.h"
#include "QuickEditor.h"
#include "QuickEditor_Internal.h"

TMap<UClass*, UFunction*> UQEFactoryNew::NewFunctionMap;
TMap<UClass*, UClass*> UQEFactoryNew::SupportClassMap;
TMap<UClass*, UFunction*> UQEFactoryFile::NewFunctionMap;
TMap<UClass*, UClass*> UQEFactoryFile::SupportClassMap;

UQEFactoryNew::UQEFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	auto Fun = NewFunctionMap.Find(GetClass());
	if (Fun)
	{
		SupportedClass = SupportClassMap[GetClass()];
	}
	else
	{
		// check(!ObjectInitializer.GetObj()->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));
		SupportedClass = UObject::StaticClass();
	}
	
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UQEFactoryNew::FactoryCreateNew(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn)
{
	QE::NewObjectClass = InClass;
	QE::NewObjectOuter = InParent;
	QE::NewObjectName = InName;
	QE::NewObjectFlag = Flags;
	QE::AssetNew::AssetNewState(true);

	UFunction* Func = NewFunctionMap[GetClass()];
	FFrame Stack(nullptr, Func, nullptr);
	Func->Invoke(nullptr, Stack, nullptr);
	
	QE::AssetNew::AssetNewState(false);
	
	return QE::NewCreatedObject;
}

UQEFactoryFile::UQEFactoryFile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	auto Fun = NewFunctionMap.Find(GetClass());
	if (Fun)
	{
		SupportedClass = SupportClassMap[GetClass()];
	}
	else
	{
		// check(!ObjectInitializer.GetObj()->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject));
		SupportedClass = UObject::StaticClass();
	}

	if (GetClass()->ClassDefaultObject)
	{
		Formats = ((UQEFactoryFile*)GetClass()->ClassDefaultObject)->Formats;
	}
	
	bCreateNew = false;
	bEditorImport = true;
}

void UQEFactoryFile::GetSupportedFileExtensions(TArray<FString>& OutExtensions) const
{
	Super::GetSupportedFileExtensions(OutExtensions);
}

UObject* UQEFactoryFile::FactoryCreateFile(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	const FString& Filename,
	const TCHAR* Parms,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	QE::NewObjectClass = InClass;
	QE::NewObjectOuter = InParent;
	QE::NewObjectName = InName;
	QE::NewObjectFlag = Flags;
	QE::ImportFileName = Filename;
	QE::AssetNew::AssetNewState(true);

	UFunction* Func = NewFunctionMap[GetClass()];
	FFrame Stack(nullptr, Func, nullptr);
	Func->Invoke(nullptr, Stack, nullptr);
	
	QE::AssetNew::AssetNewState(false);
	
	return QE::NewCreatedObject;
}
