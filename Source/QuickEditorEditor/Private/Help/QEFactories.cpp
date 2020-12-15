#include "QEFactories.h"

UQEFactoryNew::UQEFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UObject::StaticClass();
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
	return nullptr;
}

UQEFactoryFile::UQEFactoryFile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UObject::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
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
	return nullptr;
}
